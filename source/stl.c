/***************************************************************************
 *
 *  Copyright (C) 2023 CutDigital Enterprise Ltd
 *  Licensed under the GNU GPL License, Version 3.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://www.gnu.org/licenses/gpl-3.0.html.
 *
 *  For your convenience, a copy of the License has been included in this
 *  repository.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * stl.c
 *
 * \brief:
 *  TODO: ...
 *
 * Author(s):
 *
 *    Floyd M. Chitalu    CutDigital Enterprise Ltd.
 *
 **************************************************************************/

#include "mio/stl.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum StlFileCmdType
{
	SOLID,
	FACET_NORMAL,
	OUTER_LOOP,
	VERTEX,
	END_LOOP,
	END_FACET,
	END_SOLID,
	TOTAL,
	UNKNOWN = 0xFFFFFFFF
};

void mioReadSTL(
	// absolute path to file
	const char* fpath,
	// pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
	double** pVertices,
	// pointer to list of normal coordinates associated with each face stored as [xyz,xyz,xyz,...]
	// NOTE: the number of normals is V * 3, where V is the number of vertices in "pVertices"
	double** pNormals,
	// number of vertices (which can be used to deduce the number of vertices)
	unsigned int* numVertices)
{
	fprintf(stdout, "read .stl file: %s\n", fpath);

	FILE* file = fopen(fpath, "rb"); // open our file

	if(file == NULL)
	{
		fprintf(stderr, "error: failed to open file '%s'", fpath);
		exit(1);
	}

	fpos_t startOfFile;

	if(fgetpos(file, &startOfFile) != 0) /* current position: start of file */
	{
		perror("fgetpos()");
		fprintf(stderr, "error fgetpos() failed in file %s at line # %d\n", __FILE__, __LINE__ - 3);
		exit(EXIT_FAILURE);
	}

	// buffer used to store the contents of a line read from the file.
	char* lineBuf = NULL;
	// current length of the line buffer (in characters read)
	size_t lineBufLen = 0;

	// counts the number of passes we have made over the file to parse contents.
	// this is needed because multiple passes are required e.g. to determine how
	// much memory to allocate before actually copying data into pointers.
	//
	// The first pass is a survey of the file, counting the number of elements
	// (vertices, faces etc) and allocating the respective memory.
	// The second pass actually copies the data from file and into the allocated arrays

	int passIterator = 0;

	do
	{ // each iteration will parse the file in full

		// these variables are defined after having parsed the full file
		int nVertices = 0; // number of vertex coordinates found in file
		int nNormals = 0;
		int verticesCounter = 0;
		//int solidCommandCounter = 0;

		// number of characters read on a lineBuf
		size_t nread = 0;

		while((nread = getline(&lineBuf, &lineBufLen, file)) != (((size_t)0) - 1) /*-1*/)
		{ // each iteration will parse a line in the file
			//printf("line : ");
			//printf(lineBuf);

			lineBuf[strcspn(lineBuf, "\r\n")] = '\0'; // strip newline and carriage return

			const size_t lineLen = strlen(lineBuf);

			assert(lineLen <= nread);

			const bool lineIsEmpty = (lineLen == 0);

			if(lineIsEmpty)
			{
				continue; // .. skip to next line
			}

			//
			// In the following, we determine the type of "command" in the stl
			// file that is contained on the current line.
			//

			enum StlFileCmdType cmdType = UNKNOWN;

			if(strstr(lineBuf, "solid") != NULL)
			{
				cmdType = SOLID;
			}
			else if(strstr(lineBuf, "facet normal") != NULL)
			{
				cmdType = FACET_NORMAL;
			}
			else if(strstr(lineBuf, "outer loop") != NULL)
			{
				cmdType = OUTER_LOOP;
			}
			else if(strstr(lineBuf, "vertex") != NULL)
			{
				cmdType = VERTEX;
			}
			else if(strstr(lineBuf, "endloop") != NULL)
			{
				cmdType = END_LOOP;
			}
			else if(strstr(lineBuf, "endfacet") != NULL)
			{
				cmdType = END_FACET;
			}
			else if(strstr(lineBuf, "endsolid") != NULL)
			{
				cmdType = END_SOLID;
			}
			else
			{
				assert(cmdType == UNKNOWN);
				fprintf(stderr, "note: skipping unrecognised command in line '%s'\n", lineBuf);
				continue; // ... to next lineBuf
			}

			//
			// Now that we know the type of command whose data is on the current line,
			// we can go ahead and parse the line in a command-specific way
			//
			switch(cmdType)
			{
			case SOLID: {
				if(passIterator == 1)
				{
					// store offset
					//pVertexOffsets[solidCommandCounter] = nVertices; // current number of vertices parsed
				}

				//solidCommandCounter += 1;
			}
			break;
			case FACET_NORMAL: {
				const int normalId = nNormals++; // incremental vertex-normal count in file

				if(passIterator == 1)
				{

					double x = 0.0;
					double y = 0.0;
					double z = 0.0;

					nread = sscanf(lineBuf + strlen("facet normal"), "%lf %lf %lf", &x, &y, &z);

					(*pNormals)[normalId * 3 + 0] = x;
					(*pNormals)[normalId * 3 + 1] = y;
					(*pNormals)[normalId * 3 + 2] = z;

					if(nread != 3)
					{
						fprintf(stderr, "error: have %zu components for vn%d\n", nread, normalId);
						abort();
					}
				}
			}
			break;
			case OUTER_LOOP: {
				// do nothing
			}
			break;
			case VERTEX: { // parsing vertex coordinates
				const int vertexId = nVertices++; // incremental vertex count in file

				if(passIterator == 1)
				{
					double x = 0.0;
					double y = 0.0;
					double z = 0.0;

					nread = sscanf(lineBuf + strlen("vertex"), "%lf %lf %lf", &x, &y, &z);

					(*pVertices)[vertexId * 3 + 0] = x;
					(*pVertices)[vertexId * 3 + 1] = y;
					(*pVertices)[vertexId * 3 + 2] = z;

					if(nread != 3)
					{
						fprintf(stderr, "error: have %zu components for v%d\n", nread, vertexId);
						abort();
					}
				}
			}
			break;
			case END_LOOP: {
				// do nothing
			}
			break;
			case END_FACET: {
				// do nothing
			}
			break;
			case END_SOLID: {
				// do nothing
			}
			break;
			default:
				break;
			} // switch (cmdType) {
		}

		if(passIterator == 0) // first pass
		{
			printf("\t%d vertices\n", nVertices);

			assert(nNormals == nVertices / 3);

			printf("\t%d normals\n", nNormals);

			if(nVertices > 0)
			{
				// allocate user pointers
				*pVertices = (double*)malloc(nVertices * sizeof(double) * 3);
				*pNormals = (double*)malloc(nNormals * sizeof(double) * 3);
				//*pVertexOffsets = (unsigned int*)malloc(solidCommandCounter * sizeof(unsigned int));
			}

			*numVertices = nVertices;
		}

		if(fsetpos(file, &startOfFile) != 0) /* reset current position to start of file */
		{
			if(ferror(file))
			{
				perror("fsetpos()");
				fprintf(stderr,
						"fsetpos() failed in file %s at lineBuf # %d\n",
						__FILE__,
						__LINE__ - 5);
				exit(EXIT_FAILURE);
			}
		}
	} while(++passIterator < 2);

	//
	// finish, and free up memory
	//
	if(lineBuf != NULL)
	{
		free(lineBuf);
	}

	fclose(file);

	printf("done.\n");
}

void mioWriteSTL(
	// absolute path to file
	const char* const fpath,
	// pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
	const double* const pVertices,
	// pointer to list of normal coordinates associated with each face stored as [xyz,xyz,xyz,...]
	// NOTE: the number of normals is V * 3, where V is the number of vertices in "pVertices"
	const double* const pNormals,
	// number of vertices (which can be used to deduce the number of triangles)
	const unsigned int numVertices)
{
	fprintf(stdout, "write .obj file: %s\n", fpath);

	FILE* file = fopen(fpath, "w"); // open our file

	if(file == NULL)
	{
		fprintf(stderr, "error: failed to open file '%s'", fpath);
		return; // exit(1);
	}

	printf("vertices %u\n", numVertices);

	fprintf(file, "solid Unnamed\n");

	// for each vertex
	for(unsigned int j = 0u; j < (unsigned int)numVertices; ++j)
	{
		if((j % 3) == 0) // every three vertices correspond to a triangle/normal
		{
			const unsigned int n = j / 3u; // normal index
			fprintf(file,
					"facet normal %f %f %f\n",
					pNormals[n * 3u + 0u],
					pNormals[n * 3u + 1u],
					pNormals[n * 3u + 2u]);

			fprintf(file, "outer loop\n"); // mark start of triangle
		}

		const double x = pVertices[j * 3u + 0u];
		const double y = pVertices[j * 3u + 1u];
		const double z = pVertices[j * 3u + 2u];

		fprintf(file, "vertex  %f %f %f\n", x, y, z);

		if(((j + 1) % 3) == 0) // last vertex of triangle
		{
			fprintf(file, "endloop\nendfacet\n"); // mark end of triangle

			if(j == numVertices - 1)
			{
				fprintf(file, "endsolid\n"); // mark end of solid
			}
		}
	}

	fclose(file);

	printf("done.\n");
}

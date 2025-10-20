/***************************************************************************
 * Copyright (C) 2023 CutDigital Enterprise Ltd
 * Licensed under the GNU GPL License, Version 3.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html.
 *
 * For your convenience, a copy of the License has been included in this
 * repository.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * stl.c - Improved version
 *
 * \brief: Robust STL file parser with improved error handling and memory safety
 *
 * Author(s): Floyd M. Chitalu CutDigital Enterprise Ltd.
 **************************************************************************/

#include "mio/stl.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER)
#	include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

/* STL file command types for ASCII format */
typedef enum
{
	STL_CMD_SOLID,
	STL_CMD_FACET_NORMAL,
	STL_CMD_OUTER_LOOP,
	STL_CMD_VERTEX,
	STL_CMD_END_LOOP,
	STL_CMD_END_FACET,
	STL_CMD_END_SOLID,
	STL_CMD_UNKNOWN
} StlCommandType;

/* Safe memory allocation with error checking */
static void* safe_malloc(size_t size)
{
	if(size == 0)
	{
		return NULL;
	}
	void* ptr = malloc(size);
	if(ptr == NULL)
	{
		fprintf(stderr, "error: memory allocation of %zu bytes failed\n", size);
		exit(EXIT_FAILURE);
	}
	return ptr;
}

/* Detect if STL file is binary or ASCII format */
static bool is_binary_stl(FILE* file)
{
	char header[6] = {0};

	/* Save current position */
	long pos = ftell(file);
	if(pos < 0)
	{
		return false;
	}

	/* Read first 5 bytes */
	if(fread(header, 1, 5, file) != 5)
	{
		fseek(file, pos, SEEK_SET);
		return false;
	}

	/* Reset position */
	fseek(file, pos, SEEK_SET);

	/* Binary STL should NOT start with "solid" */
	return (strncmp(header, "solid", 5) != 0);
}

/* Parse ASCII STL command type from line */
static StlCommandType parse_stl_command(const char* line)
{
	if(strstr(line, "facet normal") != NULL)
	{
		return STL_CMD_FACET_NORMAL;
	}
	else if(strstr(line, "outer loop") != NULL)
	{
		return STL_CMD_OUTER_LOOP;
	}
	else if(strstr(line, "endloop") != NULL)
	{
		return STL_CMD_END_LOOP;
	}
	else if(strstr(line, "endfacet") != NULL)
	{
		return STL_CMD_END_FACET;
	}
	else if(strstr(line, "endsolid") != NULL)
	{
		return STL_CMD_END_SOLID;
	}
	else if(strstr(line, "vertex") != NULL)
	{
		return STL_CMD_VERTEX;
	}
	else if(strstr(line, "solid") != NULL)
	{
		return STL_CMD_SOLID;
	}
	return STL_CMD_UNKNOWN;
}

/* Read binary STL file */
static void
read_binary_stl(FILE* file, double** pVertices, double** pNormals, unsigned int* numVertices)
{
	/* Skip 80-byte header */
	if(fseek(file, 80, SEEK_SET) != 0)
	{
		fprintf(stderr, "error: failed to skip STL binary header\n");
		return;
	}

	/* Read triangle count (4-byte little-endian unsigned integer) */
	uint32_t triangleCount = 0;
	if(fread(&triangleCount, sizeof(uint32_t), 1, file) != 1)
	{
		fprintf(stderr, "error: failed to read triangle count\n");
		return;
	}

	fprintf(stdout, "\t%u triangles\n", triangleCount);

	if(triangleCount == 0)
	{
		fprintf(stderr, "warning: STL file contains no triangles\n");
		return;
	}

	/* Allocate arrays */
	const unsigned int nVertices = triangleCount * 3;
	const unsigned int nNormals = triangleCount;

	*numVertices = nVertices;
	*pVertices = (double*)safe_malloc(sizeof(double) * nVertices * 3);
	*pNormals = (double*)safe_malloc(sizeof(double) * nNormals * 3);

	fprintf(stdout, "\t%u vertices\n", nVertices);
	fprintf(stdout, "\t%u normals\n", nNormals);

	/* Read each triangle */
	for(uint32_t i = 0; i < triangleCount; i++)
	{
		/* Normal vector (3 floats) */
		float normal[3];
		if(fread(normal, sizeof(float), 3, file) != 3)
		{
			fprintf(stderr, "error: failed to read normal for triangle %u\n", i);
			return;
		}

		(*pNormals)[i * 3 + 0] = (double)normal[0];
		(*pNormals)[i * 3 + 1] = (double)normal[1];
		(*pNormals)[i * 3 + 2] = (double)normal[2];

		/* Three vertices (9 floats total) */
		for(int v = 0; v < 3; v++)
		{
			float vertex[3];
			if(fread(vertex, sizeof(float), 3, file) != 3)
			{
				fprintf(stderr, "error: failed to read vertex %d of triangle %u\n", v, i);
				return;
			}

			const unsigned int vertexIdx = i * 3 + v;
			(*pVertices)[vertexIdx * 3 + 0] = (double)vertex[0];
			(*pVertices)[vertexIdx * 3 + 1] = (double)vertex[1];
			(*pVertices)[vertexIdx * 3 + 2] = (double)vertex[2];
		}

		/* Attribute byte count (2 bytes, usually zero) */
		uint16_t attributeByteCount = 0;
		if(fread(&attributeByteCount, sizeof(uint16_t), 1, file) != 1)
		{
			fprintf(stderr, "error: failed to read attribute byte count for triangle %u\n", i);
			return;
		}

		/* Some software uses attribute bytes for color or other data */
		if(attributeByteCount != 0)
		{
			fprintf(stdout,
					"\tnote: triangle %u has non-zero attribute byte count: %u\n",
					i,
					attributeByteCount);
		}
	}
}

/* Read ASCII STL file */
static void
read_ascii_stl(FILE* file, double** pVertices, double** pNormals, unsigned int* numVertices)
{
	char* lineBuf = NULL;
	size_t lineBufLen = 0;

	/* First pass: count vertices and normals */
	fprintf(stdout, "\tFirst pass: counting elements...\n");

	int nVertices = 0;
	int nNormals = 0;

	while(getline(&lineBuf, &lineBufLen, file) != (ssize_t)-1)
	{
		/* Strip trailing whitespace */
		lineBuf[strcspn(lineBuf, "\r\n")] = '\0';

		size_t lineLen = strlen(lineBuf);
		if(lineLen == 0)
		{
			continue;
		}

		StlCommandType cmdType = parse_stl_command(lineBuf);

		switch(cmdType)
		{
		case STL_CMD_FACET_NORMAL:
			nNormals++;
			break;
		case STL_CMD_VERTEX:
			nVertices++;
			break;
		default:
			break;
		}
	}

	fprintf(stdout, "\t%d vertices\n", nVertices);
	fprintf(stdout, "\t%d normals\n", nNormals);

	if(nVertices == 0 || nNormals == 0)
	{
		fprintf(stderr, "warning: no geometry found in STL file\n");
		free(lineBuf);
		return;
	}

	/* Validate: should be 3 vertices per normal */
	if(nVertices != nNormals * 3)
	{
		fprintf(stderr,
				"warning: vertex count (%d) is not 3x normal count (%d)\n",
				nVertices,
				nNormals);
	}

	/* Allocate memory */
	*numVertices = nVertices;
	*pVertices = (double*)safe_malloc(sizeof(double) * nVertices * 3);
	*pNormals = (double*)safe_malloc(sizeof(double) * nNormals * 3);

	/* Second pass: parse data */
	fprintf(stdout, "\tSecond pass: parsing data...\n");
	rewind(file);

	int vertexIdx = 0;
	int normalIdx = 0;

	while(getline(&lineBuf, &lineBufLen, file) != (ssize_t)-1)
	{
		lineBuf[strcspn(lineBuf, "\r\n")] = '\0';

		size_t lineLen = strlen(lineBuf);
		if(lineLen == 0)
		{
			continue;
		}

		StlCommandType cmdType = parse_stl_command(lineBuf);

		switch(cmdType)
		{
		case STL_CMD_FACET_NORMAL: {
			double x, y, z;
			const char* dataStart = strstr(lineBuf, "facet normal");
			if(dataStart != NULL)
			{
				dataStart += strlen("facet normal");
				if(sscanf(dataStart, "%lf %lf %lf", &x, &y, &z) == 3)
				{
					(*pNormals)[normalIdx * 3 + 0] = x;
					(*pNormals)[normalIdx * 3 + 1] = y;
					(*pNormals)[normalIdx * 3 + 2] = z;
					normalIdx++;
				}
				else
				{
					fprintf(stderr, "error: failed to parse normal at index %d\n", normalIdx);
				}
			}
			break;
		}

		case STL_CMD_VERTEX: {
			double x, y, z;
			const char* dataStart = strstr(lineBuf, "vertex");
			if(dataStart != NULL)
			{
				dataStart += strlen("vertex");
				if(sscanf(dataStart, "%lf %lf %lf", &x, &y, &z) == 3)
				{
					(*pVertices)[vertexIdx * 3 + 0] = x;
					(*pVertices)[vertexIdx * 3 + 1] = y;
					(*pVertices)[vertexIdx * 3 + 2] = z;
					vertexIdx++;
				}
				else
				{
					fprintf(stderr, "error: failed to parse vertex at index %d\n", vertexIdx);
				}
			}
			break;
		}

		default:
			break;
		}
	}

	free(lineBuf);
}

void mioReadSTL(const char* fpath, double** pVertices, double** pNormals, unsigned int* numVertices)
{
	fprintf(stdout, "read .stl file: %s\n", fpath);

	/* Initialize output parameters */
	*pVertices = NULL;
	*pNormals = NULL;
	*numVertices = 0;

	/* Open file in binary mode */
	FILE* file = fopen(fpath, "rb");
	if(file == NULL)
	{
		fprintf(stderr, "error: failed to open file '%s'\n", fpath);
		return;
	}

	/* Detect format */
	bool isBinary = is_binary_stl(file);
	fprintf(stdout, "\tformat: %s\n", isBinary ? "binary" : "ASCII");

	/* Parse based on format */
	if(isBinary)
	{
		read_binary_stl(file, pVertices, pNormals, numVertices);
	}
	else
	{
		read_ascii_stl(file, pVertices, pNormals, numVertices);
	}

	fclose(file);
	fprintf(stdout, "done.\n");
}

void mioWriteSTL(const char* fpath,
				 const double* pVertices,
				 const double* pNormals,
				 const unsigned int numVertices)
{
	fprintf(stdout, "write .stl file: %s\n", fpath);

	if(pVertices == NULL || numVertices == 0)
	{
		fprintf(stderr, "error: no vertices to write\n");
		return;
	}

	if(pNormals == NULL)
	{
		fprintf(stderr, "error: no normals to write\n");
		return;
	}

	/* Validate: vertices should be multiple of 3 */
	if(numVertices % 3 != 0)
	{
		fprintf(stderr, "warning: vertex count (%u) is not a multiple of 3\n", numVertices);
	}

	FILE* file = fopen(fpath, "w");
	if(file == NULL)
	{
		fprintf(stderr, "error: failed to open '%s' for writing\n", fpath);
		return;
	}

	const unsigned int numTriangles = numVertices / 3;
	fprintf(stdout, "\twriting %u triangles (%u vertices)\n", numTriangles, numVertices);

	/* Write header */
	fprintf(file, "solid Exported\n");

	/* Write each triangle */
	for(unsigned int i = 0; i < numTriangles; i++)
	{
		/* Write normal */
		fprintf(file,
				"  facet normal %.6f %.6f %.6f\n",
				pNormals[i * 3 + 0],
				pNormals[i * 3 + 1],
				pNormals[i * 3 + 2]);

		fprintf(file, "    outer loop\n");

		/* Write three vertices */
		for(unsigned int v = 0; v < 3; v++)
		{
			const unsigned int vertexIdx = i * 3 + v;
			fprintf(file,
					"      vertex %.6f %.6f %.6f\n",
					pVertices[vertexIdx * 3 + 0],
					pVertices[vertexIdx * 3 + 1],
					pVertices[vertexIdx * 3 + 2]);
		}

		fprintf(file, "    endloop\n");
		fprintf(file, "  endfacet\n");
	}

	/* Write footer */
	fprintf(file, "endsolid Exported\n");

	fclose(file);
	fprintf(stdout, "done.\n");
}

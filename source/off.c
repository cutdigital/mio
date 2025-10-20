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
 * off.c 
 *
 * \brief: Robust OFF file parser with improved error handling and memory safety
 *
 * Author(s): Floyd M. Chitalu CutDigital Enterprise Ltd.
 **************************************************************************/

#include "mio/off.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER)
#	include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

// Safe memory allocation with error checking
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

// Read a non-empty, non-comment line from file
static bool read_line(FILE* file, char** line, size_t* len)
{
	ssize_t read;
	while((read = getline(line, len, file)) != -1)
	{
		// Strip trailing whitespace
		size_t lineLen = strlen(*line);
		while(lineLen > 0 && ((*line)[lineLen - 1] == '\n' || (*line)[lineLen - 1] == '\r' ||
							  (*line)[lineLen - 1] == ' ' || (*line)[lineLen - 1] == '\t'))
		{
			(*line)[--lineLen] = '\0';
		}

		// Skip empty lines and comments
		if(lineLen > 0 && (*line)[0] != '#')
		{
			return true;
		}
	}
	return false;
}

// Parse face vertex indices from a line
static bool parse_face_indices(const char* line, unsigned int* indices, unsigned int expectedCount)
{
	const char* ptr = line;
	char* end;

	// Skip the first number (vertex count)
	strtol(ptr, &end, 10);
	if(end == ptr)
	{
		return false;
	}
	ptr = end;

	// Parse the vertex indices
	for(unsigned int i = 0; i < expectedCount; i++)
	{
		// Skip whitespace
		while(*ptr == ' ' || *ptr == '\t')
		{
			ptr++;
		}
		if(*ptr == '\0')
		{
			return false;
		}

		long val = strtol(ptr, &end, 10);
		if(end == ptr)
		{
			return false;
		}
		indices[i] = (unsigned int)val;
		ptr = end;
	}

	return true;
}

void mioReadOFF(const char* fpath,
				double** pVertices,
				unsigned int** pFaceVertexIndices,
				unsigned int** pFaceSizes,
				unsigned int* numVertices,
				unsigned int* numFaces)
{
	fprintf(stdout, "read OFF file: %s\n", fpath);

	// Initialize output parameters
	*pVertices = NULL;
	*pFaceVertexIndices = NULL;
	*pFaceSizes = NULL;
	*numVertices = 0;
	*numFaces = 0;

	// Open file in binary mode to prevent linefeed conversion
	// See: https://stackoverflow.com/questions/27530636
	FILE* file = fopen(fpath, "rb");
	if(file == NULL)
	{
		fprintf(stderr, "error: failed to open '%s'\n", fpath);
		return;
	}

	char* line = NULL;
	size_t lineBufLen = 0;

	// Read and validate file header
	if(!read_line(file, &line, &lineBufLen))
	{
		fprintf(stderr, "error: .off file header not found\n");
		goto cleanup;
	}

	if(strstr(line, "OFF") == NULL)
	{
		fprintf(stderr, "error: unrecognised .off file header (expected 'OFF', got '%s')\n", line);
		goto cleanup;
	}

	// Read element counts: vertices, faces, edges
	if(!read_line(file, &line, &lineBufLen))
	{
		fprintf(stderr, "error: .off element count not found\n");
		goto cleanup;
	}

	int nedges = 0;
	unsigned int nVerts = 0, nFaces = 0;
	if(sscanf(line, "%u %u %d", &nVerts, &nFaces, &nedges) < 2)
	{
		fprintf(stderr, "error: failed to parse element counts\n");
		goto cleanup;
	}

	if(nVerts == 0 || nFaces == 0)
	{
		fprintf(
			stderr, "error: invalid element counts (vertices: %u, faces: %u)\n", nVerts, nFaces);
		goto cleanup;
	}

	fprintf(stdout, "\t%u vertices\n", nVerts);
	fprintf(stdout, "\t%u faces\n", nFaces);
	fprintf(stdout, "\t%d edges\n", nedges);

	// Allocate vertex and face size arrays
	*pVertices = (double*)safe_malloc(sizeof(double) * nVerts * 3);
	*pFaceSizes = (unsigned int*)safe_malloc(sizeof(unsigned int) * nFaces);

	// Read vertices
	fprintf(stdout, "\tReading vertices...\n");
	for(unsigned int i = 0; i < nVerts; i++)
	{
		if(!read_line(file, &line, &lineBufLen))
		{
			fprintf(stderr, "error: expected vertex at line %u\n", i);
			goto cleanup;
		}

		double x, y, z;
		if(sscanf(line, "%lf %lf %lf", &x, &y, &z) != 3)
		{
			fprintf(stderr, "error: failed to parse vertex %u\n", i);
			goto cleanup;
		}

		(*pVertices)[i * 3 + 0] = x;
		(*pVertices)[i * 3 + 1] = y;
		(*pVertices)[i * 3 + 2] = z;
	}

	// First pass through faces: count total indices
	fprintf(stdout, "\tFirst pass: counting face indices...\n");

#ifdef _WIN64
	__int64 facesStartOffset = _ftelli64(file);
#else
	long int facesStartOffset = ftell(file);
#endif

	if(facesStartOffset < 0)
	{
		fprintf(stderr, "error: ftell failed\n");
		goto cleanup;
	}

	unsigned int numFaceIndices = 0;
	for(unsigned int i = 0; i < nFaces; i++)
	{
		if(!read_line(file, &line, &lineBufLen))
		{
			fprintf(stderr, "error: expected face at index %u\n", i);
			goto cleanup;
		}

		int faceSize = 0;
		if(sscanf(line, "%d", &faceSize) != 1 || faceSize < 3)
		{
			fprintf(stderr, "error: invalid face size %d at face %u\n", faceSize, i);
			goto cleanup;
		}

		(*pFaceSizes)[i] = (unsigned int)faceSize;
		numFaceIndices += faceSize;
	}

	fprintf(stdout, "\t%u total face indices\n", numFaceIndices);

	// Allocate face vertex indices array
	*pFaceVertexIndices = (unsigned int*)safe_malloc(sizeof(unsigned int) * numFaceIndices);

	// Seek back to start of faces
#ifdef _WIN64
	int seekResult = _fseeki64(file, facesStartOffset, SEEK_SET);
#else
	int seekResult = fseek(file, facesStartOffset, SEEK_SET);
#endif

	if(seekResult != 0)
	{
		fprintf(stderr, "error: fseek failed\n");
		goto cleanup;
	}

	// Second pass through faces: read indices
	fprintf(stdout, "\tSecond pass: reading face indices...\n");
	unsigned int indexOffset = 0;
	for(unsigned int i = 0; i < nFaces; i++)
	{
		if(!read_line(file, &line, &lineBufLen))
		{
			fprintf(stderr, "error: expected face at index %u\n", i);
			goto cleanup;
		}

		unsigned int faceSize = (*pFaceSizes)[i];

		if(!parse_face_indices(line, *pFaceVertexIndices + indexOffset, faceSize))
		{
			fprintf(stderr, "error: failed to parse face %u indices\n", i);
			goto cleanup;
		}

		// Validate indices
		for(unsigned int j = 0; j < faceSize; j++)
		{
			unsigned int idx = (*pFaceVertexIndices)[indexOffset + j];
			if(idx >= nVerts)
			{
				fprintf(stderr,
						"warning: face %u has out-of-range vertex index %u (max: %u)\n",
						i,
						idx,
						nVerts - 1);
			}
		}

		indexOffset += faceSize;
	}

	// Success: set output parameters
	*numVertices = nVerts;
	*numFaces = nFaces;

	free(line);
	fclose(file);
	fprintf(stdout, "done.\n");
	return;

cleanup:
	// Error cleanup: free any allocated memory
	free(line);
	fclose(file);

	free(*pVertices);
	free(*pFaceSizes);
	free(*pFaceVertexIndices);

	*pVertices = NULL;
	*pFaceSizes = NULL;
	*pFaceVertexIndices = NULL;
	*numVertices = 0;
	*numFaces = 0;
}

void mioWriteOFF(const char* fpath,
				 double* pVertices,
				 unsigned int* pFaceVertexIndices,
				 unsigned int* pFaceSizes,
				 unsigned int* pEdgeVertexIndices,
				 unsigned int numVertices,
				 unsigned int numFaces,
				 unsigned int numEdges)
{
	fprintf(stdout, "write OFF file: %s\n", fpath);

	FILE* file = fopen(fpath, "w");
	if(file == NULL)
	{
		fprintf(stderr, "error: failed to open '%s' for writing\n", fpath);
		return;
	}

	// Write header
	fprintf(file, "OFF\n");
	fprintf(file, "%u %u %u\n", numVertices, numFaces, numEdges);

	// Write vertices
	fprintf(stdout, "\tWriting %u vertices\n", numVertices);
	for(unsigned int i = 0; i < numVertices; i++)
	{
		const double* vptr = pVertices + (i * 3);
		fprintf(file, "%.6f %.6f %.6f\n", vptr[0], vptr[1], vptr[2]);
	}

	// Write faces
	fprintf(stdout, "\tWriting %u faces\n", numFaces);
	unsigned int indexBase = 0;
	for(unsigned int i = 0; i < numFaces; i++)
	{
		const unsigned int faceVertexCount = (pFaceSizes != NULL) ? pFaceSizes[i] : 3;
		fprintf(file, "%u", faceVertexCount);

		for(unsigned int j = 0; j < faceVertexCount; j++)
		{
			const unsigned int idx = pFaceVertexIndices[indexBase + j];
			fprintf(file, " %u", idx);
		}

		fprintf(file, "\n");
		indexBase += faceVertexCount;
	}

	// Write edges (if provided)
	if(numEdges > 0 && pEdgeVertexIndices != NULL)
	{
		fprintf(stdout, "\tWriting %u edges\n", numEdges);
		for(unsigned int i = 0; i < numEdges; i++)
		{
			const unsigned int* eptr = pEdgeVertexIndices + (i * 2);
			fprintf(file, "%u %u\n", eptr[0], eptr[1]);
		}
	}

	fclose(file);
	fprintf(stdout, "done.\n");
}

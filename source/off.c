#include "mio/off.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool readLine(FILE* file, char** line, size_t* len)
{
	while(getline(line, len, file))
	{
		if(strlen(*line) > 1 && (*line)[0] != '#')
		{
			return true;
		}
	}
	return false;
}

void mioReadOFF(const char* fpath,
				double** pVertices,
				unsigned int** pFaceVertexIndices,
				unsigned int** pFaceSizes,
				unsigned int* numVertices,
				unsigned int* numFaces)
{
	printf("read OFF file %s: \n", fpath);

	// using "rb" instead of "r" to prevent linefeed conversion
	// See:
	// https://stackoverflow.com/questions/27530636/read-text-file-in-c-with-fopen-without-linefeed-conversion
	FILE* file = fopen(fpath, "rb");

	if(file == NULL)
	{
		fprintf(stderr, "error: failed to open `%s`", fpath);
		exit(1);
	}

	char* line = NULL;
	size_t lineBufLen = 0;
	bool lineOk = true;
	int i = 0;

	// file header
	lineOk = readLine(file, &line, &lineBufLen);

	if(!lineOk)
	{
		fprintf(stderr, "error: .off file header not found\n");
		exit(1);
	}

	if(strstr(line, "OFF") == NULL)
	{
		fprintf(stderr, "error: unrecognised .off file header\n");
		exit(1);
	}

	// #vertices, #faces, #edges
	lineOk = readLine(file, &line, &lineBufLen);

	if(!lineOk)
	{
		fprintf(stderr, "error: .off element count not found\n");
		exit(1);
	}

	int nedges = 0;
	sscanf(line, "%d %d %d", numVertices, numFaces, &nedges);
	*pVertices = (double*)malloc(sizeof(double) * (*numVertices) * 3);
	*pFaceSizes = (unsigned int*)malloc(sizeof(unsigned int) * (*numFaces));

	// vertices
	for(i = 0; i < (double)(*numVertices); ++i)
	{
		lineOk = readLine(file, &line, &lineBufLen);

		if(!lineOk)
		{
			fprintf(stderr, "error: .off vertex not found\n");
			exit(1);
		}

		double x, y, z;
		sscanf(line, "%lf %lf %lf", &x, &y, &z);

		(*pVertices)[(i * 3) + 0] = x;
		(*pVertices)[(i * 3) + 1] = y;
		(*pVertices)[(i * 3) + 2] = z;
	}
#if _WIN64
	__int64 facesStartOffset = _ftelli64(file);
#else
	long int facesStartOffset = ftell(file);
#endif
	int numFaceIndices = 0;

	// faces
	for(i = 0; i < (int)(*numFaces); ++i)
	{
		lineOk = readLine(file, &line, &lineBufLen);

		if(!lineOk)
		{
			fprintf(stderr, "error: .off file face not found\n");
			exit(1);
		}

		int n; // number of vertices in face
		sscanf(line, "%d", &n);

		if(n < 3)
		{
			fprintf(stderr, "error: invalid vertex count in file %d\n", n);
			exit(1);
		}

		(*pFaceSizes)[i] = n;
		numFaceIndices += n;
	}

	(*pFaceVertexIndices) = (unsigned int*)malloc(sizeof(unsigned int) * numFaceIndices);

#if _WIN64
	int api_err = _fseeki64(file, facesStartOffset, SEEK_SET);
#else
	int api_err = fseek(file, facesStartOffset, SEEK_SET);
#endif
	if(api_err != 0)
	{
		fprintf(stderr, "error: fseek failed\n");
		exit(1);
	}

	int indexOffset = 0;
	for(i = 0; i < (int)(*numFaces); ++i)
	{

		lineOk = readLine(file, &line, &lineBufLen);

		if(!lineOk)
		{
			fprintf(stderr, "error: .off file face not found\n");
			exit(1);
		}

		int n; // number of vertices in face
		sscanf(line, "%d", &n);

		char* lineBufShifted = line;
		int j = 0;

		while(j < n)
		{ // parse remaining numbers on line
			lineBufShifted = strstr(lineBufShifted, " ") + 1; // start of next number

			int val;
			sscanf(lineBufShifted, "%d", &val);

			(*pFaceVertexIndices)[indexOffset + j] = val;
			j++;
		}

		indexOffset += n;
	}

	free(line);

	fclose(file);
}

// To ignore edges when writing the output just pass pEdgeVertexIndices = NULL and set numEdges = 0
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
		fprintf(stderr, "error: failed to open `%s`", fpath);
		exit(1);
	}

	fprintf(file, "OFF\n");
	fprintf(file, "%d %d %d\n", numVertices, numFaces, numEdges);

	int i;

	for(i = 0; i < (int)numVertices; ++i)
	{
		const double* vptr = pVertices + (i * 3);
		fprintf(file, "%f %f %f\n", vptr[0], vptr[1], vptr[2]);
	}

	int base = 0;

	for(i = 0; i < (int)numFaces; ++i)
	{

		const unsigned int faceVertexCount = (pFaceSizes != NULL) ? pFaceSizes[i] : 3;
		fprintf(file, "%d", (int)faceVertexCount);
		int j;

		for(j = 0; j < (int)faceVertexCount; ++j)
		{
			const unsigned int* fptr = pFaceVertexIndices + base + j;
			fprintf(file, " %d", *fptr);
		}

		fprintf(file, "\n");

		base += faceVertexCount;
	}

	for(i = 0; i < (int)numEdges; ++i)
	{
		const unsigned int* eptr = pEdgeVertexIndices + (i * 2);
		fprintf(file, "%u %u\n", eptr[0], eptr[1]);
	}

	fclose(file);
}

/***************************************************************************
 *
 *  Copyright (C) 2024 CutDigital Enterprise Ltd
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  For your convenience, a copy of the License has been included in this
 *  repository.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.WE
 *
 **************************************************************************/

#include "mio/mio.h"

#include <assert.h>
#include <stdlib.h>

#if defined(_WIN32)
#	include <stdlib.h> // https://stackoverflow.com/questions/44504429/c-write-access-violation
ssize_t getline(char** lineptr, size_t* n, FILE* stream)
{
	size_t pos = 0;
	int c = 0;

	if(lineptr == NULL || stream == NULL || n == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	c = getc(stream);
	if(c == EOF)
	{
		return -1;
	}

	if(*lineptr == NULL)
	{
		*lineptr = (char*)malloc(128);
		if(*lineptr == NULL)
		{
			return -1;
		}
		*n = 128;
	}

	pos = 0;
	while(c != EOF)
	{
		if(pos + 1 >= *n)
		{
			size_t new_size = *n + (*n >> 2);
			if(new_size < 128)
			{
				new_size = 128;
			}
			char* new_ptr = (char*)realloc(*lineptr, new_size);
			if(new_ptr == NULL)
			{
				return -1;
			}
			*n = new_size;
			*lineptr = new_ptr;
		}

		((unsigned char*)(*lineptr))[pos++] = (unsigned char)c;
		if(c == '\n')
		{
			break;
		}
		c = getc(stream);
	}

	(*lineptr)[pos] = '\0';
	return pos;
}

/*
    Function to read in a mesh file. Supported file formats are .obj and .off
*/
void mioRead(
	// absolute path to file
	const char* fpath,
	// pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
	double** pVertices,
	// pointer to list of face sizes (number of vertices in each face) stored as [a,b,c,d,e,f,g,...]
	unsigned int** pFaceSizes,
	// pointer to list of face-vertex indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...]
	unsigned int** pFaceVertexIndices,
	// number of vertices in "pVertices"
	unsigned int* numVertices,
	// number of faces
	unsigned int* numFaces)
{
	// TODO
}

void mioWrite(
	// absolute path to file
	const char* fpath,
	// pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
	double** pVertices,
	// pointer to list of face sizes (number of vertices in each face) stored as [a,b,c,d,e,f,g,...]
	unsigned int** pFaceSizes,
	// pointer to list of face-vertex indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...]
	unsigned int** pFaceVertexIndices,
	// number of vertices in "pVertices"
	unsigned int* numVertices,
	// number of faces
	unsigned int* numFaces)
{
	// TODO
}

#endif // #if defined (_WIN32)

void mioFree(void* pMemPtr)
{
	if(pMemPtr != NULL)
	{
		free(pMemPtr);
	}
}

void mioFreeMesh(MioMesh* pMeshPtr)
{
	assert(pMeshPtr != NULL);

	mioFree(pMeshPtr->pVertices);
	pMeshPtr->pVertices = NULL;
	mioFree(pMeshPtr->pNormals);
	pMeshPtr->pNormals = NULL;
	mioFree(pMeshPtr->pTexCoords);
	pMeshPtr->pTexCoords = NULL;
	mioFree(pMeshPtr->pFaceSizes);
	pMeshPtr->pFaceSizes = NULL;
	mioFree(pMeshPtr->pFaceVertexIndices);
	pMeshPtr->pFaceVertexIndices = NULL;
	mioFree(pMeshPtr->pFaceVertexTexCoordIndices);
	pMeshPtr->pFaceVertexTexCoordIndices = NULL;
	mioFree(pMeshPtr->pFaceVertexNormalIndices);
	pMeshPtr->pFaceVertexNormalIndices = NULL;

	pMeshPtr->numVertices = 0;
	pMeshPtr->numNormals = 0;
	pMeshPtr->numTexCoords = 0;
	pMeshPtr->numFaces = 0;
}


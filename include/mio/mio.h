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
 * mio.h
 *
 * \brief:
 *  TODO: ...
 *
 * Author(s):
 *
 *    Floyd M. Chitalu    CutDigital Enterprise Ltd.
 *
 **************************************************************************/

#ifndef _MIO_H_
#define _MIO_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
    
#include "mio/obj.h"
#include "mio/off.h"
#include "mio/ply.h"
#include "mio/stl.h"

#if defined(_WIN32)
// https://stackoverflow.com/questions/735126/are-there-alternate-implementations-of-gnu-getline-interface/735472#735472
/* Modifications, public domain as well, by Antti Haapala, 11/10/17
- Switched to getc on 5/23/19 */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
// if typedef doesn't exist (msvc)
typedef intptr_t ssize_t;
ssize_t getline(char** lineptr, size_t* n, FILE* stream);
#endif // #if defined(_WIN32)

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
    unsigned int* numFaces
);

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
    unsigned int* numFaces
);

// frees the memory associated with the given pointer and sets the pointer to NULL.
// NOTE: pMemPtr must be the address of a pointer that was internally allocated by "mio"
void mioFree(void** pMemPtr);

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef _MIO_H_
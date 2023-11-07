//
//  CUTDIGITAL ENTERPRISE LTD PROPRIETARY INFORMATION
//
//  This software is supplied under the terms of a separately provided license agreement
//  with CutDigital Enterprise Ltd and may not be copied or exploited except in 
//  accordance with the terms of that agreement.
//
//  THIS SOFTWARE IS PROVIDED BY CUTDIGITAL ENTERPRISE LTD "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CUTDIGITAL ENTERPRISE LTD BE LIABLE 
//  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
//  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  Copyright (c) 2023 CutDigital Enterprise Ltd. All rights reserved in all media.
//

#ifndef _MIO_H_
#define _MIO_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

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

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef _MIO_H_
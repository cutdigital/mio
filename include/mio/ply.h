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
 * ply.h
 *
 * \brief:
 *  TODO: ...
 *
 * Author(s):
 *
 *    Floyd M. Chitalu    CutDigital Enterprise Ltd.
 *
 **************************************************************************/

#ifndef __MIO_PLY_H__
#define __MIO_PLY_H__  1

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*
    Funcion to read in a .ply file that stores a single 3D mesh object (in ASCII
    format). The pointer parameters will be allocated inside this function and must
    be freed by caller. 

    NOTE: Current implementation only reads in the vertices and faces. Thus only 
    the following variables will be defined when the function returns:
    - pVertices
    - pFaceSizes
    - pFaceVertexIndices
    - numVertices
    - numFaces

    Vertices are read in as floats!
*/
void mioReadPLY(
    // absolute path to file
    const char* fpath,
    // pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
    double** pVertices,
    // pointer to list of vertex normals stored as [xyz,xyz,xyz...]
    double** pNormals,
    // pointer to list of texture coordinates list stored as [xy,xy,xy...]
    double** pTexCoords,
    // pointer to list of face sizes (number of vertices in each face) stored as [a,b,c,d,e,f,g,...]
    unsigned int** pFaceSizes,
    // pointer to list of face-vertex indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...]
    unsigned int** pFaceVertexIndices,
    // pointer to list of face-vertex texture-coord indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...] (same order as in "pFaceVertexIndices")
    unsigned int** pFaceVertexTexCoordIndices,
    // pointer to list of face texture coordvertex-normal indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...] (same order as in "pFaceVertexIndices")
    unsigned int** pFaceVertexNormalIndices,
    // number of vertices in "pVertices"
    unsigned int* numVertices,
    // number of vertex normals in "pNormals"
    unsigned int* numNormals,
    // number of texture coordinates in "pTexCoords"
    unsigned int* numTexcoords,
    // number of faces
    unsigned int* numFaces);

/*
    Funcion to write out a .ply file that stores a single 3D mesh object (in ASCII
    format).

    NOTE: Current implementation only writes out the vertices and faces. Thus only 
    the following variables will be used.
    - pVertices
    - pFaceSizes
    - pFaceVertexIndices
    - numVertices
    - numFaces

    Vertices are written out as floats!
*/
void mioWritePLY(
    // absolute path to file
    const char* fpath,
    // pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
    double* pVertices,
    // pointer to list of vertex normals stored as [xyz,xyz,xyz...]
    double* pNormals,
    // pointer to list of texture coordinates list stored as [xy,xy,xy...]
    double* pTexCoords,
    // pointer to list of face sizes (number of vertices in each face) stored as [a,b,c,d,e,f,g,...]
    unsigned int* pFaceSizes,
    // pointer to list of face-vertex indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...]
    unsigned int* pFaceVertexIndices,
    // pointer to list of face-vertex texture-coord indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...] (same order as in "pFaceVertexIndices")
    unsigned int* pFaceVertexTexCoordIndices,
    // pointer to list of face texture coordvertex-normal indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...] (same order as in "pFaceVertexIndices")
    unsigned int* pFaceVertexNormalIndices,
    // number of vertices in "pVertices"
    unsigned int numVertices,
    // number of vertex normals in "pNormals"
    unsigned int numNormals,
    // number of texture coordinates in "pTexCoords"
    unsigned int numTexcoords,
    // number of faces (number of element in "pFaceSizes")
    unsigned int numFaces);


#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef __MIO_PLY_H__
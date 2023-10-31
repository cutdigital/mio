/// This Source Code Form is subject to the terms of the Mozilla Public
/// License, v. 2.0. If a copy of the MPL was not distributed with this
/// file, You can obtain one at https://mozilla.org/MPL/2.0/.
/// Contribution of original implementation:
/// Floyd M. Chitalu <floyd.m.chitalu@gmail.com>

#ifndef _MIO_H_
#define _MIO_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

#if 0
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
#endif

///////////////////////////////////////////////////////////////////////////////
// OBJ files
///////////////////////////////////////////////////////////////////////////////

/*
    Funcion to read in an obj file that stores a single 3D mesh object (in ASCII
    format). The pointer parameters will be allocated inside this function and must
    be freed by caller. The function only handles polygonal faces, so commands like
    "vp" command (which is used to specify control points of the surface or curve)
    are ignored if encountered in file.

*/
void mioReadOBJ(
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
    Funcion to write out an obj file that stores a single 3D mesh object (in ASCII
    format).
*/
void mioWriteOBJ(
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

///////////////////////////////////////////////////////////////////////////////
// OFF files
///////////////////////////////////////////////////////////////////////////////

/*
    Funcion to read in an .off file that stores a single 3D mesh object (in ASCII
    format). The pointer parameters will be allocated inside this function and must
    be freed by caller.
*/
void mioReadOFF(
    // absolute path to file
    const char* fpath,
    // pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
    double** pVertices,
    // pointer to list of face-vertex indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...]
    unsigned int** pFaceVertexIndices,
    // pointer to list of face sizes (number of vertices in each face) stored as [a,b,c,d,e,f,g,...]
    unsigned int** pFaceSizes,
    // number of vertices in "pVertices"
    unsigned int* numVertices,
    // number of faces (number of elements in "pFaceSizes")
    unsigned int* numFaces);

/*
    Funcion to write out a .obj file that stores a single 3D mesh object (in ASCII
    format). 
    NOTE: To ignore edges when writing the output just pass pEdgeVertexIndices = NULL and set numEdges = 0
*/
void mioWriteOFF(
    // absolute path to file
    const char* fpath,
    // pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
    double* pVertices,
    // pointer to list of face-vertex indices stored as [ijkl,ijk,ijkl,ijklmn,ijk,...]
    unsigned int* pFaceVertexIndices,
    // pointer to list of face sizes (number of vertices in each face) stored as [a,b,c,d,e,f,g,...]
    unsigned int* pFaceSizes,
    // pointer to list of edge-vertex indices stored as [ij,ij,ij,ij,ij,...]
    unsigned int* pEdgeVertexIndices,
    // number of vertices in "pVertices"
    unsigned int numVertices,
    // number of faces (number of elements in "pFaceSizes")
    unsigned int numFaces,
    // number of edges (number of elements in "pEdgeVertexIndices" divided by 2)
    unsigned int numEdges);

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef _MIO_H_
#include "mio/ply.h"
#include "mio/third-party/ply.h"

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
    unsigned int* numFaces)
    {
        // TODO:
    }

/*
    Funcion to write out a .ply file that stores a single 3D mesh object (in ASCII
    format).
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
    unsigned int numFaces){
        // TODO:
    }
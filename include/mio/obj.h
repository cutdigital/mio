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

#ifndef __MIO_OBJ_H__
#define __MIO_OBJ_H__  1

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

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

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef __MIO_OBJ_H__
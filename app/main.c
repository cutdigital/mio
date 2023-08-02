#include "mio/mio.h"

#include <stdio.h>
#include <stdlib.h>

#define STR(x) #x
#define ASSERT(x)                                                                                                     \
    if (!(x)) {                                                                                                       \
        printf("Assertion failed: (%s), function %s, file %s, line %d.\n", STR(x), __FUNCTION__, __FILE__, __LINE__); \
        abort();                                                                                                      \
    }

int main()
{
    double* pVertices = NULL;
    double* pNormals = NULL;
    double* pTexCoords = NULL;
    unsigned int* pFaceSizes = NULL;
    unsigned int* pFaceVertexIndices = NULL;
    unsigned int* pFaceVertexTexCoordIndices = NULL;
    unsigned int* pFaceVertexNormalIndices = NULL;
    unsigned int numVertices = 0;
    unsigned int numNormals = 0;
    unsigned int numTexCoords = 0;
    unsigned int numFaces = 0;

    { // mioReadOBJ (only vertices and faces)

        mioReadOBJ(DATA_DIR "/cube.obj", &pVertices, &pNormals,
            &pTexCoords, &pFaceSizes, &pFaceVertexIndices, &pFaceVertexTexCoordIndices,
            &pFaceVertexNormalIndices, &numVertices, &numNormals,
            &numTexCoords, &numFaces);

        ASSERT(pVertices != NULL);
        ASSERT(pFaceSizes != NULL);
        ASSERT(pFaceVertexIndices != NULL);
        ASSERT(numVertices == 8);
        ASSERT(numFaces == 12);

        mioWriteOBJ("cube-out.obj", pVertices, pNormals,
            pTexCoords, pFaceSizes, pFaceVertexIndices, pFaceVertexTexCoordIndices,
            pFaceVertexNormalIndices, numVertices, numNormals,
            numTexCoords, numFaces);

        if (pVertices != NULL) {
            free(pVertices);
            pVertices = NULL;
        }

        if (pNormals != NULL) {
            free(pNormals);
            pNormals = NULL;
        }

        if (pTexCoords != NULL) {
            free(pTexCoords);
            pTexCoords = NULL;
        }

        if (pFaceSizes != NULL) {
            free(pFaceSizes);
            pFaceSizes = NULL;
        }

        if (pFaceVertexIndices != NULL) {
            free(pFaceVertexIndices);
            pFaceVertexIndices = NULL;
        }

        if (pFaceVertexTexCoordIndices != NULL) {
            free(pFaceVertexTexCoordIndices);
            pFaceVertexTexCoordIndices = NULL;
        }

        if (pFaceVertexNormalIndices != NULL) {
            free(pFaceVertexNormalIndices);
            pFaceVertexNormalIndices = NULL;
        }

        numVertices = 0;
        numNormals = 0;
        numTexCoords = 0;
        numFaces = 0;
    }

    { // mioReadOBJ (vertices, texcoords and faces)

        mioReadOBJ(DATA_DIR "/cube-uv.obj", &pVertices, &pNormals,
            &pTexCoords, &pFaceSizes, &pFaceVertexIndices, &pFaceVertexTexCoordIndices,
            &pFaceVertexNormalIndices, &numVertices, &numNormals,
            &numTexCoords, &numFaces);

        ASSERT(pVertices != NULL);
        ASSERT(pTexCoords != NULL);
        ASSERT(pFaceSizes != NULL);
        ASSERT(pFaceVertexIndices != NULL);
        ASSERT(pFaceVertexTexCoordIndices != NULL);
        ASSERT(numVertices == 8);
        ASSERT(numTexCoords == 14);
        ASSERT(numFaces == 12);

        mioWriteOBJ("cube-uv-out.obj", pVertices, pNormals,
            pTexCoords, pFaceSizes, pFaceVertexIndices, pFaceVertexTexCoordIndices,
            pFaceVertexNormalIndices, numVertices, numNormals,
            numTexCoords, numFaces);

        if (pVertices != NULL) {
            free(pVertices);
            pVertices = NULL;
        }

        if (pNormals != NULL) {
            free(pNormals);
            pNormals = NULL;
        }

        if (pTexCoords != NULL) {
            free(pTexCoords);
            pTexCoords = NULL;
        }

        if (pFaceSizes != NULL) {
            free(pFaceSizes);
            pFaceSizes = NULL;
        }

        if (pFaceVertexIndices != NULL) {
            free(pFaceVertexIndices);
            pFaceVertexIndices = NULL;
        }

        if (pFaceVertexTexCoordIndices != NULL) {
            free(pFaceVertexTexCoordIndices);
            pFaceVertexTexCoordIndices = NULL;
        }

        if (pFaceVertexNormalIndices != NULL) {
            free(pFaceVertexNormalIndices);
            pFaceVertexNormalIndices = NULL;
        }

        numVertices = 0;
        numNormals = 0;
        numTexCoords = 0;
        numFaces = 0;
    }

    { // mioReadOBJ (vertices, normals and faces)

        mioReadOBJ(DATA_DIR "/cube-normals.obj", &pVertices, &pNormals,
            &pTexCoords, &pFaceSizes, &pFaceVertexIndices, &pFaceVertexTexCoordIndices,
            &pFaceVertexNormalIndices, &numVertices, &numNormals,
            &numTexCoords, &numFaces);

        ASSERT(pVertices != NULL);
        ASSERT(pNormals != NULL);
        ASSERT(pFaceSizes != NULL);
        ASSERT(pFaceVertexIndices != NULL);
        ASSERT(pFaceVertexNormalIndices != NULL);
        ASSERT(numVertices == 8);
        ASSERT(numNormals == 6);
        ASSERT(numFaces == 12);

        mioWriteOBJ("cube-normals-out.obj", pVertices, pNormals,
            pTexCoords, pFaceSizes, pFaceVertexIndices, pFaceVertexTexCoordIndices,
            pFaceVertexNormalIndices, numVertices, numNormals,
            numTexCoords, numFaces);

        if (pVertices != NULL) {
            free(pVertices);
            pVertices = NULL;
        }

        if (pNormals != NULL) {
            free(pNormals);
            pNormals = NULL;
        }

        if (pTexCoords != NULL) {
            free(pTexCoords);
            pTexCoords = NULL;
        }

        if (pFaceSizes != NULL) {
            free(pFaceSizes);
            pFaceSizes = NULL;
        }

        if (pFaceVertexIndices != NULL) {
            free(pFaceVertexIndices);
            pFaceVertexIndices = NULL;
        }

        if (pFaceVertexTexCoordIndices != NULL) {
            free(pFaceVertexTexCoordIndices);
            pFaceVertexTexCoordIndices = NULL;
        }

        if (pFaceVertexNormalIndices != NULL) {
            free(pFaceVertexNormalIndices);
            pFaceVertexNormalIndices = NULL;
        }

        numVertices = 0;
        numNormals = 0;
        numTexCoords = 0;
        numFaces = 0;
    }

    { // mioReadOBJ (vertices, texture coordinates, normals and faces)

        mioReadOBJ(DATA_DIR "/cube-normals-uv.obj", &pVertices, &pNormals,
            &pTexCoords, &pFaceSizes, &pFaceVertexIndices, &pFaceVertexTexCoordIndices,
            &pFaceVertexNormalIndices, &numVertices, &numNormals,
            &numTexCoords, &numFaces);

        ASSERT(pVertices != NULL);
        ASSERT(pNormals != NULL);
        ASSERT(pTexCoords != NULL);
        ASSERT(pFaceSizes != NULL);
        ASSERT(pFaceVertexIndices != NULL);
        ASSERT(pFaceVertexTexCoordIndices != NULL);
        ASSERT(pFaceVertexNormalIndices != NULL);
        ASSERT(numVertices == 8);
        ASSERT(numNormals == 6);
        ASSERT(numTexCoords == 14);
        ASSERT(numFaces == 12);

        mioWriteOBJ("cube-normals-uv-out.obj", pVertices, pNormals,
            pTexCoords, pFaceSizes, pFaceVertexIndices, pFaceVertexTexCoordIndices,
            pFaceVertexNormalIndices, numVertices, numNormals,
            numTexCoords, numFaces);

        if (pVertices != NULL) {
            free(pVertices);
            pVertices = NULL;
        }

        if (pNormals != NULL) {
            free(pNormals);
            pNormals = NULL;
        }

        if (pTexCoords != NULL) {
            free(pTexCoords);
            pTexCoords = NULL;
        }

        if (pFaceSizes != NULL) {
            free(pFaceSizes);
            pFaceSizes = NULL;
        }

        if (pFaceVertexIndices != NULL) {
            free(pFaceVertexIndices);
            pFaceVertexIndices = NULL;
        }

        if (pFaceVertexTexCoordIndices != NULL) {
            free(pFaceVertexTexCoordIndices);
            pFaceVertexTexCoordIndices = NULL;
        }

        if (pFaceVertexNormalIndices != NULL) {
            free(pFaceVertexNormalIndices);
            pFaceVertexNormalIndices = NULL;
        }

        numVertices = 0;
        numNormals = 0;
        numTexCoords = 0;
        numFaces = 0;
    }

    { // mioReadOBJ (only vertices and quad-faces)

        mioReadOBJ(DATA_DIR "/cube-quads.obj", &pVertices, &pNormals,
            &pTexCoords, &pFaceSizes, &pFaceVertexIndices, &pFaceVertexTexCoordIndices,
            &pFaceVertexNormalIndices, &numVertices, &numNormals,
            &numTexCoords, &numFaces);

        ASSERT(pVertices != NULL);
        ASSERT(pFaceSizes != NULL);
        ASSERT(pFaceVertexIndices != NULL);
        ASSERT(numVertices == 8);
        ASSERT(numFaces == 6);

        mioWriteOBJ("cube-quads-out.obj", pVertices, pNormals,
            pTexCoords, pFaceSizes, pFaceVertexIndices, pFaceVertexTexCoordIndices,
            pFaceVertexNormalIndices, numVertices, numNormals,
            numTexCoords, numFaces);

        if (pVertices != NULL) {
            free(pVertices);
            pVertices = NULL;
        }

        if (pNormals != NULL) {
            free(pNormals);
            pNormals = NULL;
        }

        if (pTexCoords != NULL) {
            free(pTexCoords);
            pTexCoords = NULL;
        }

        if (pFaceSizes != NULL) {
            free(pFaceSizes);
            pFaceSizes = NULL;
        }

        if (pFaceVertexIndices != NULL) {
            free(pFaceVertexIndices);
            pFaceVertexIndices = NULL;
        }

        if (pFaceVertexTexCoordIndices != NULL) {
            free(pFaceVertexTexCoordIndices);
            pFaceVertexTexCoordIndices = NULL;
        }

        if (pFaceVertexNormalIndices != NULL) {
            free(pFaceVertexNormalIndices);
            pFaceVertexNormalIndices = NULL;
        }

        numVertices = 0;
        numNormals = 0;
        numTexCoords = 0;
        numFaces = 0;
    }

    { // mioReadOBJ (vertices, normals and quad-faces)

        mioReadOBJ(DATA_DIR "/cube-quads-normals.obj", &pVertices, &pNormals,
            &pTexCoords, &pFaceSizes, &pFaceVertexIndices, &pFaceVertexTexCoordIndices,
            &pFaceVertexNormalIndices, &numVertices, &numNormals,
            &numTexCoords, &numFaces);

        ASSERT(pVertices != NULL);
        ASSERT(pNormals != NULL);
        ASSERT(pFaceSizes != NULL);
        ASSERT(pFaceVertexIndices != NULL);
        ASSERT(pFaceVertexNormalIndices != NULL);
        ASSERT(numVertices == 8);
        ASSERT(numNormals == 6);
        ASSERT(numFaces == 6);

        mioWriteOBJ("cube-quads-normals-out.obj", pVertices, pNormals,
            pTexCoords, pFaceSizes, pFaceVertexIndices, pFaceVertexTexCoordIndices,
            pFaceVertexNormalIndices, numVertices, numNormals,
            numTexCoords, numFaces);

        if (pVertices != NULL) {
            free(pVertices);
            pVertices = NULL;
        }

        if (pNormals != NULL) {
            free(pNormals);
            pNormals = NULL;
        }

        if (pTexCoords != NULL) {
            free(pTexCoords);
            pTexCoords = NULL;
        }

        if (pFaceSizes != NULL) {
            free(pFaceSizes);
            pFaceSizes = NULL;
        }

        if (pFaceVertexIndices != NULL) {
            free(pFaceVertexIndices);
            pFaceVertexIndices = NULL;
        }

        if (pFaceVertexTexCoordIndices != NULL) {
            free(pFaceVertexTexCoordIndices);
            pFaceVertexTexCoordIndices = NULL;
        }

        if (pFaceVertexNormalIndices != NULL) {
            free(pFaceVertexNormalIndices);
            pFaceVertexNormalIndices = NULL;
        }

        numVertices = 0;
        numNormals = 0;
        numTexCoords = 0;
        numFaces = 0;
    }

    return 0;
}
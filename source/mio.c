/// This Source Code Form is subject to the terms of the Mozilla Public
/// License, v. 2.0. If a copy of the MPL was not distributed with this
/// file, You can obtain one at https://mozilla.org/MPL/2.0/.
/// Contribution of original implementation:
/// Floyd M. Chitalu <floyd.m.chitalu@gmail.com>

#include "mio/mio.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// OFF
///////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)

// https://stackoverflow.com/questions/735126/are-there-alternate-implementations-of-gnu-getline-interface/735472#735472
/* Modifications, public domain as well, by Antti Haapala, 11/10/17
- Switched to getc on 5/23/19 */
#include <errno.h>
#include <stdint.h>

// if typedef doesn't exist (msvc, blah)
typedef intptr_t ssize_t;

ssize_t getline(char** lineptr, size_t* n, FILE* stream)
{
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    c = getc(stream);
    if (c == EOF) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = (char*)malloc(128);
        if (*lineptr == NULL) {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while (c != EOF) {
        if (pos + 1 >= *n) {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char* new_ptr = (char*)realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char*)(*lineptr))[pos++] = (unsigned char)c;
        if (c == '\n') {
            break;
        }
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}
#endif // #if defined (_WIN32)

bool readLine(FILE* file, char** line, size_t* len)
{
    while (getline(line, len, file)) {
        if (strlen(*line) > 1 && (*line)[0] != '#') {
            return true;
        }
    }
    return false;
}

void mioReadOFF(const char* fpath, double** pVertices, unsigned int** pFaceVertexIndices,
    unsigned int** pFaceSizes, unsigned int* numVertices,
    unsigned int* numFaces)
{
    printf("read OFF file %s: \n", fpath);

    // using "rb" instead of "r" to prevent linefeed conversion
    // See:
    // https://stackoverflow.com/questions/27530636/read-text-file-in-c-with-fopen-without-linefeed-conversion
    FILE* file = fopen(fpath, "rb");

    if (file == NULL) {
        fprintf(stderr, "error: failed to open `%s`", fpath);
        exit(1);
    }

    char* line = NULL;
    size_t lineBufLen = 0;
    bool lineOk = true;
    int i = 0;

    // file header
    lineOk = readLine(file, &line, &lineBufLen);

    if (!lineOk) {
        fprintf(stderr, "error: .off file header not found\n");
        exit(1);
    }

    if (strstr(line, "OFF") == NULL) {
        fprintf(stderr, "error: unrecognised .off file header\n");
        exit(1);
    }

    // #vertices, #faces, #edges
    lineOk = readLine(file, &line, &lineBufLen);

    if (!lineOk) {
        fprintf(stderr, "error: .off element count not found\n");
        exit(1);
    }

    int nedges = 0;
    sscanf(line, "%d %d %d", numVertices, numFaces, &nedges);
    *pVertices = (double*)malloc(sizeof(double) * (*numVertices) * 3);
    *pFaceSizes = (unsigned int*)malloc(sizeof(unsigned int) * (*numFaces));

    // vertices
    for (i = 0; i < (double)(*numVertices); ++i) {
        lineOk = readLine(file, &line, &lineBufLen);

        if (!lineOk) {
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
    for (i = 0; i < (int)(*numFaces); ++i) {
        lineOk = readLine(file, &line, &lineBufLen);

        if (!lineOk) {
            fprintf(stderr, "error: .off file face not found\n");
            exit(1);
        }

        int n; // number of vertices in face
        sscanf(line, "%d", &n);

        if (n < 3) {
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
    if (api_err != 0) {
        fprintf(stderr, "error: fseek failed\n");
        exit(1);
    }

    int indexOffset = 0;
    for (i = 0; i < (int)(*numFaces); ++i) {

        lineOk = readLine(file, &line, &lineBufLen);

        if (!lineOk) {
            fprintf(stderr, "error: .off file face not found\n");
            exit(1);
        }

        int n; // number of vertices in face
        sscanf(line, "%d", &n);

        char* lineBufShifted = line;
        int j = 0;

        while (j < n) { // parse remaining numbers on line
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
void mioWriteOFF(
    const char* fpath,
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

    if (file == NULL) {
        fprintf(stderr, "error: failed to open `%s`", fpath);
        exit(1);
    }

    fprintf(file, "OFF\n");
    fprintf(file, "%d %d %d\n", numVertices, numFaces, numEdges);

    int i;

    for (i = 0; i < (int)numVertices; ++i) {
        const double* vptr = pVertices + (i * 3);
        fprintf(file, "%f %f %f\n", vptr[0], vptr[1], vptr[2]);
    }

    int base = 0;

    for (i = 0; i < (int)numFaces; ++i) {

        const unsigned int faceVertexCount = (pFaceSizes != NULL) ? pFaceSizes[i] : 3;
        fprintf(file, "%d", (int)faceVertexCount);
        int j;

        for (j = 0; j < (int)faceVertexCount; ++j) {
            const unsigned int* fptr = pFaceVertexIndices + base + j;
            fprintf(file, " %d", *fptr);
        }

        fprintf(file, "\n");

        base += faceVertexCount;
    }

    for (i = 0; i < (int)numEdges; ++i) {
        const unsigned int* eptr = pEdgeVertexIndices + (i * 2);
        fprintf(file, "%u %u\n", eptr[0], eptr[1]);
    }

    fclose(file);
}

///////////////////////////////////////////////////////////////////////////////
// OBJ
///////////////////////////////////////////////////////////////////////////////

enum ObjFileCmdType {
    /*
    The v command specifies a vertex by its three cartesian coordinates x, y, and
    z. The vertex is automatically assigned a name depending on the order in which
    it is found in the file. The first vertex in the file gets the name ‘1’, the
    second ‘2’, the third ‘3’, and so on.
    */
    VERTEX,
    /*
    This is the vertex normal command. It specifies the normal vector to the
    surface. x, y, and z are the components of the normal vector. Note that this
    normal vector is not yet associated with any vertex point. We will have to
    associate it with a vertex later with another command called the f command.

    The vertex normal command is typically omitted in files because when we group
    vertices into polygonal faces with the f command, it will automatically
    determine the normal vector from the vertex coordinates and the order in which
    the vertices appear.
    */
    NORMAL,
    /*
    The vertex texture command specifies a point in the texture map, which we
    covered in an earlier section. u and v are the x and y coordinates in the
    texture map. These will be floating point numbers between 0 and 1. They really
    don’t tell you anything by themselves, they must be grouped with a vertex in
    an f face command, just like the vertex normals.
    */
    TEXCOORD,
    /*
    The face command is probably the most important command. It specifies a
    polygonal face made from the vertices that came before this line.

    To reference a vertex you follow the implicit numbering system of the
    vertices. For example’ f 23 24 25 27′ means a polygonal face built from
    vertices 23, 24, 25, and 27 in order.

    For each vertex, you may associate a vn command, which then associates that
    vertex normal to the corresponding vertex. Similarly, you can associate a vt
    command with a vertex, which will determine the texture mapping to use at this
    point.

    If you specify a vt or vn for a vertex in a file, you must specify it for all
    of them.
    */
    FACE,
    TOTAL,
    UNKNOWN = 0xFFFFFFFF
};

// Funcion to read in an obj file that stores a single 3D mesh object (in ASCII
// format). The pointer parameters will be allocated inside this function and must
// be freed by caller. The function only handles polygonal faces, so commands like
// "vp" command (which is used to specify control points of the surface or curve)
// are ignored if encountered in file.
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
    unsigned int* numFaces)
{

    fprintf(stdout, "read .obj file: %s\n", fpath);

    FILE* file = fopen(fpath, "rb"); // open our file

    if (file == NULL) {
        fprintf(stderr, "error: failed to open file '%s'", fpath);
        exit(1);
    }

    fpos_t startOfFile;

    if (fgetpos(file, &startOfFile) != 0) /* current position: start of file */
    {
        perror("fgetpos()");
        fprintf(stderr, "error fgetpos() failed in file %s at line # %d\n",
            __FILE__, __LINE__ - 3);
        exit(EXIT_FAILURE);
    }

    // buffer used to store the contents of a line read from the file.
    char* lineBuf = NULL;
    // current length of the line buffer (in characters read)
    size_t lineBufLen = 0;

    // counts the number of passes we have made over the file to parse contents.
    // this is needed because multiple passes are required e.g. to determine how
    // much memory to allocate before actually copying data into pointers.
    //
    // The first pass is a survey of the file, counting the number of elements
    // (vertices, faces etc) and allocating the respective memory.
    // The second pass actually copies the data from file and into the allocated arrays
    // (most of them). Also count the total number of face indices.
    // The third pass is specifically for copying face index data after knowing the total
    // number of face indices.
    int passIterator = 0;

    int nFaceIndices = 0; // total number of face indices found in file

    do { // each iteration will parse the file in full

        // these variables are defined after having parsed the full file
        int nVertices = 0; // number of vertex coordinates found in file
        int nNormals = 0; // number of vertex normals found in file
        int nTexCoords = 0; // number of vertex vertex texture coordnates found in file
        int nFaces = 0; // number of faces found in file

        int faceIndicesCounter = 0; // running offset into pFaceVertexIndices (final value
                                    // will be same as nFaceIndices)

        // number of characters read on a lineBuf
        size_t nread = 0;

        while ((nread = getline(&lineBuf, &lineBufLen, file)) != (((size_t)0) - 1) /*-1*/) { // each iteration will parse a line in the file

            lineBuf[strcspn(lineBuf, "\r\n")] = '\0'; // strip newline and carriage return

            const size_t lineLen = strlen(lineBuf);

            assert(lineLen <= nread);

            const bool lineIsEmpty = (lineLen == 0);

            if (lineIsEmpty) {
                continue; // .. skip to next line
            }

            const bool lineIsComment = lineBuf[0] == '#';

            if (lineIsComment) {
                continue; // ... skip to next line
            }

            //
            // In the following, we determine the type of "command" in the object
            // file that is contained on the current line.
            //

            enum ObjFileCmdType cmdType = UNKNOWN;

            if (lineBuf[0] == 'v' && lineBuf[1] == ' ') {
                cmdType = VERTEX;
            } else if (lineBuf[0] == 'v' && lineBuf[1] == 'n' && lineBuf[2] == ' ') {
                cmdType = NORMAL;
            } else if (lineBuf[0] == 'v' && lineBuf[1] == 't' && lineBuf[2] == ' ') {
                cmdType = TEXCOORD;
            } else if (lineBuf[0] == 'f' && lineBuf[1] == ' ') {
                cmdType = FACE;
            } else {
                assert(cmdType == UNKNOWN);
                fprintf(stderr, "note: skipping unrecognised command in line '%s'\n", lineBuf);
                continue; // ... to next lineBuf
            }

            //
            // Now that we know the type of command whose data is on the current line,
            // we can go ahead and parse the line in a command-specific way
            //
            switch (cmdType) {
            case VERTEX: { // parsing vertex coordinates
                const int vertexId = nVertices++; // incremental vertex count in file

                if (passIterator == 1) {

                    double x = 0.0;
                    double y = 0.0;
                    double z = 0.0;

                    nread = sscanf(lineBuf + 2, "%lf %lf %lf", &x, &y, &z);

                    (*pVertices)[vertexId * 3 + 0] = x;
                    (*pVertices)[vertexId * 3 + 1] = y;
                    (*pVertices)[vertexId * 3 + 2] = z;

                    if (nread != 3) {
                        fprintf(stderr, "error: have %zu components for v%d\n", nread,
                            vertexId);
                        abort();
                    }
                }

            } break;
            case NORMAL: { // parsing normal coordinates
                const int normalId = nNormals++; // incremental vertex-normal count in file

                if (passIterator == 1) {

                    double x = 0.0;
                    double y = 0.0;
                    double z = 0.0;

                    nread = sscanf(lineBuf + 2, "%lf %lf %lf", &x, &y, &z);

                    (*pNormals)[normalId * 3 + 0] = x;
                    (*pNormals)[normalId * 3 + 1] = y;
                    (*pNormals)[normalId * 3 + 2] = z;

                    if (nread != 3) {
                        fprintf(stderr, "error: have %zu components for vn%d\n", nread,
                            normalId);
                        abort();
                    }
                }
            } break;
            case TEXCOORD: { // parsing texture coordinates
                const int texCoordId = nTexCoords++; // incremental tex coord count in file

                if (passIterator == 1) {

                    double x = 0.0;
                    double y = 0.0;
                    nread = sscanf(lineBuf + 2, "%lf %lf", &x, &y);

                    (*pTexCoords)[texCoordId * 2 + 0] = x;
                    (*pTexCoords)[texCoordId * 2 + 1] = y;

                    if (nread != 2) {
                        fprintf(stderr, "error: have %zu components for vt%d\n", nread,
                            texCoordId);
                        abort();
                    }
                }
            } break;
            case FACE: { // parsing faces
                const int faceId = nFaces++; // incremental face count in file

                if (passIterator == 1) {
                    //
                    // count the number of vertices in face
                    //
                    char* pch = strtok(lineBuf + 2, " ");
                    unsigned int faceVertexCount = 0;

                    while (pch != NULL) {
                        faceVertexCount++; // track number of vertices found in face
                        pch = strtok(NULL, " ");
                    }

                    assert(pFaceSizes != NULL);

                    (*pFaceSizes)[faceId] = faceVertexCount;

                    nFaceIndices += faceVertexCount;
                } else if (passIterator == 2) // third pass
                {

                    //
                    // allocate mem for the number of vertices in face
                    //

                    assert(pFaceSizes != NULL);

                    const unsigned int faceVertexCount = (*pFaceSizes)[faceId];
                    // printf("lineBuf = %s\n", lineBuf);
                    int iter = 0; // incremented per vertex of face

                    char* token = NULL;
                    char* tokenElem = NULL;
                    char* buf = NULL; // char buf[512];

                    // for each vertex in face
                    for (token = strtok(lineBuf, " "); token != NULL;
                         token = strtok(token + strlen(token) + 1, " ")) {

                        const size_t tokenLen = strlen(token);

                        // allocate enough to accomodate all characters on the line since a face can be defined by an
                        // arbitrary number of vertices whose indices typically require several digits.
                        buf = (char*)realloc(NULL, sizeof(char) * tokenLen);

                        memset(buf, 0, tokenLen); // equivalent to filling with '\0'

                        strncpy(buf, token, tokenLen);

                        if (buf[0] == 'f') {
                            continue;
                        }

                        iter++;
                        int faceVertexDataIt = 0;

                        // printf("Line: %s\n", buf);

                        // for each data element of a face-vertex
                        for (tokenElem = strtok(buf, "/"); tokenElem != NULL;
                             tokenElem = strtok(tokenElem + strlen(tokenElem) + 1, "/")) {

                            // printf("\tToken: %s\n", tokenElem);

                            const bool haveTexCoords = (nTexCoords > 0);

                            if (faceVertexDataIt == 1 && !haveTexCoords) {
                                faceVertexDataIt = 2;
                            }

                            int val;
                            sscanf(token, "%d", &val); // extract face vertex data index

                            switch (faceVertexDataIt) {
                            case 0: // vertex id
                                (*pFaceVertexIndices)[faceIndicesCounter] = (unsigned int)(val - 1);
                                break;
                            case 1: // texcooord id
                                (*pFaceVertexTexCoordIndices)[faceIndicesCounter] = (unsigned int)(val - 1);
                                break;
                            case 2: // normal id
                                (*pFaceVertexNormalIndices)[faceIndicesCounter] = (unsigned int)(val - 1);
                                break;
                            default:
                                break;
                            }

                            faceVertexDataIt++;
                        }
                        faceIndicesCounter += 1;
                    }

                    if (iter != (int)faceVertexCount) {
                        fprintf(stderr,
                            "error: have %d vertices when there should be =%d\n", iter,
                            faceVertexCount);
                        abort();
                    }
                }
            } break;
            default:
                break;
            } // switch (cmdType) {
        }

        if (passIterator == 0) // first pass
        {

            printf("\t%d positions\n", nVertices);

            if (nVertices > 0) {
                *pVertices = (double*)malloc(nVertices * sizeof(double) * 3);
            }

            printf("\t%d normals\n", nNormals);

            if (nNormals > 0) {
                *pNormals = (double*)malloc(nNormals * sizeof(double) * 3);
            }

            printf("\t%d texture-coords\n", nTexCoords);

            if (nTexCoords > 0) {
                *pTexCoords = (double*)malloc(nTexCoords * sizeof(double) * 2);
            }

            printf("\t%d face(s)\n", nFaces);

            if (nFaces > 0) {
                *pFaceSizes = (unsigned int*)malloc(nFaces * sizeof(unsigned int));
            }

            *numVertices = nVertices;
            *numNormals = nNormals;
            *numTexcoords = nTexCoords,
            *numFaces = nFaces;
        }

        if (passIterator == 1) // second pass
        {
            printf("\t%d face indices\n", nFaceIndices);

            if (nFaceIndices == 0) {
                fprintf(stderr, "error: invalid face index count %d\n", nFaceIndices);
                abort();
            }

            *pFaceVertexIndices = (unsigned int*)malloc(nFaceIndices * sizeof(unsigned int));

            if (nTexCoords > 0) {
                printf("\t%d tex-coord indices\n", nFaceIndices);

                *pFaceVertexTexCoordIndices = (unsigned int*)malloc(nFaceIndices * sizeof(unsigned int));
            }

            if (nNormals > 0) {
                printf("\t%d normal indices\n", nFaceIndices);

                *pFaceVertexNormalIndices = (unsigned int*)malloc(nFaceIndices * sizeof(unsigned int));
            }
        }

        if (fsetpos(file, &startOfFile) != 0) /* reset current position to start of file */
        {
            if (ferror(file)) {
                perror("fsetpos()");
                fprintf(stderr, "fsetpos() failed in file %s at lineBuf # %d\n", __FILE__,
                    __LINE__ - 5);
                exit(EXIT_FAILURE);
            }
        }
    } while (++passIterator < 3);

    //
    // finish, and free up memory
    //
    if (lineBuf != NULL) {
        free(lineBuf);
    }

    fclose(file);

    printf("done.\n");
}

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
    // number of faces
    unsigned int numFaces)
{
    fprintf(stdout, "write .obj file: %s\n", fpath);

    FILE* file = fopen(fpath, "w"); // open our file

    if (file == NULL) {
        fprintf(stderr, "error: failed to open file '%s'", fpath);
        return; // exit(1);
    }

    printf("vertices %u\n", numVertices);

    // for each position
    for (unsigned int i = 0; i < (unsigned int)numVertices; ++i) {

        const double x = pVertices[i * 3u + 0u];
        const double y = pVertices[i * 3u + 1u];
        const double z = pVertices[i * 3u + 2u];

        fprintf(file, "v %f %f %f\n", x, y, z);
    }

     printf("normals %u\n", numNormals);

    // for each normal
    for (unsigned int i = 0; i < (unsigned int)numNormals; ++i) {

        const double x = pNormals[i * 3u + 0u];
        const double y = pNormals[i * 3u + 1u];
        const double z = pNormals[i * 3u + 2u];

        fprintf(file, "vn %f %f %f\n", x, y, z);
    }

    printf("texcoords %u\n", numTexcoords);

    // for each texcoord
    for (unsigned int i = 0; i < (unsigned int)numTexcoords; ++i) {

        const double s = pTexCoords[i * 3u + 0u];
        const double t = pTexCoords[i * 3u + 1u];

        fprintf(file, "vt %f %f\n", s, t);
    }

    unsigned int base = 0;
    
    printf("faces %u\n", numFaces);
    
    // for each face
    for (unsigned int f = 0; f < numFaces; ++f) {

        int faceSize = pFaceSizes[f];

        fprintf(file, "v ");

        // for each vertex in face
        for (int v = 0; v < faceSize; v++) {
            // Note: obj file indices start from 1
            const unsigned int vertexIdx = pFaceVertexIndices[base + v] + 1;

            const bool isLastVertex = (v == (faceSize - 1));

            if (numNormals > 0 && numTexcoords > 0) {
                const unsigned int normalIdx = pFaceVertexNormalIndices[base + v] + 1;
                const unsigned int texCoordIdx = pFaceVertexTexCoordIndices[base + v] + 1;
                fprintf(file, "%u/%u/%u", vertexIdx, texCoordIdx, normalIdx);
            } else if (numNormals > 0) {
                const unsigned int normalIdx = pFaceVertexNormalIndices[base + v] + 1;
                fprintf(file, "%u//%u", vertexIdx, normalIdx);
            } else if (numTexcoords > 0) {
                const unsigned int texCoordIdx = pFaceVertexTexCoordIndices[base + v] + 1;
                fprintf(file, "%u/%u", vertexIdx, texCoordIdx);
            } else {
                fprintf(file, "%u", vertexIdx);
            }

            if (!isLastVertex) {
                fprintf(file, " "); // add space between face-vertex components i.e. a/b/c d/e/f g/h/i
            }
        }

        fprintf(file, "\n");

        base += faceSize;
    }

    fclose(file);

    printf("done.\n");
}
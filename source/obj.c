/***************************************************************************
 * Copyright (C) 2023 CutDigital Enterprise Ltd
 * Licensed under the GNU GPL License, Version 3.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html.
 *
 * For your convenience, a copy of the License has been included in this
 * repository.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * obj.c - Improved version
 *
 * \brief: Robust OBJ file parser with improved error handling and memory safety
 *
 * Author(s): Floyd M. Chitalu CutDigital Enterprise Ltd.
 **************************************************************************/

#include "mio/obj.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Command types found in OBJ files
typedef enum {
    CMD_VERTEX,    // v - vertex coordinates (x, y, z)
    CMD_NORMAL,    // vn - vertex normal (x, y, z)
    CMD_TEXCOORD,  // vt - texture coordinate (u, v)
    CMD_FACE,      // f - polygonal face
    CMD_UNKNOWN
} ObjCommandType;

// Parse state for tracking progress through file
typedef struct {
    int vertexCount;
    int normalCount;
    int texCoordCount;
    int faceCount;
    int faceIndexCount;
} ParseCounts;

// Safe memory allocation with error checking
static void* safe_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "error: memory allocation of %zu bytes failed\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static void* safe_realloc(void* ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    void* new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        fprintf(stderr, "error: memory reallocation to %zu bytes failed\n", size);
        free(ptr);
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

// Identify command type from line
static ObjCommandType identify_command(const char* line) {
    if (line[0] == 'v') {
        if (line[1] == ' ') {
            return CMD_VERTEX;
        } else if (line[1] == 'n' && line[2] == ' ') {
            return CMD_NORMAL;
        } else if (line[1] == 't' && line[2] == ' ') {
            return CMD_TEXCOORD;
        }
    } else if (line[0] == 'f' && line[1] == ' ') {
        return CMD_FACE;
    }
    return CMD_UNKNOWN;
}

// Strip trailing whitespace including \r\n
static void strip_trailing_whitespace(char* line) {
    if (line == NULL) {
        return;
    }
    size_t len = strlen(line);
    while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r' || 
                       line[len - 1] == ' ' || line[len - 1] == '\t')) {
        line[--len] = '\0';
    }
}

// Parse vertex coordinates (v x y z)
static bool parse_vertex(const char* line, double* x, double* y, double* z) {
    // Skip "v " prefix
    const char* data = line + 2;
    while (*data == ' ' || *data == '\t') {
        data++;
    }
    
    char* end;
    *x = strtod(data, &end);
    if (end == data) return false;
    
    *y = strtod(end, &end);
    if (end == data) return false;
    
    *z = strtod(end, &end);
    return true;
}

// Parse texture coordinate (vt u v)
static bool parse_texcoord(const char* line, double* u, double* v) {
    const char* data = line + 3;
    while (*data == ' ' || *data == '\t') {
        data++;
    }
    
    char* end;
    *u = strtod(data, &end);
    if (end == data) return false;
    
    *v = strtod(end, &end);
    return true;
}

// Parse normal (vn x y z)
static bool parse_normal(const char* line, double* x, double* y, double* z) {
    const char* data = line + 3;
    while (*data == ' ' || *data == '\t') {
        data++;
    }
    
    char* end;
    *x = strtod(data, &end);
    if (end == data) return false;
    
    *y = strtod(end, &end);
    if (end == data) return false;
    
    *z = strtod(end, &end);
    return true;
}

// Count vertices in a face line
static unsigned int count_face_vertices(const char* line) {
    unsigned int count = 0;
    const char* ptr = line + 2; // Skip "f "
    
    while (*ptr != '\0') {
        // Skip leading whitespace
        while (*ptr == ' ' || *ptr == '\t') {
            ptr++;
        }
        if (*ptr == '\0') break;
        
        // Found start of vertex data
        count++;
        
        // Skip to next space or end
        while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t') {
            ptr++;
        }
    }
    
    return count;
}

// Parse a single face vertex entry (e.g., "1/2/3" or "1//3" or "1/2" or "1")
static bool parse_face_vertex(const char* token, int* v_idx, int* vt_idx, int* vn_idx) {
    *v_idx = -1;
    *vt_idx = -1;
    *vn_idx = -1;
    
    char* end;
    long val = strtol(token, &end, 10);
    if (end == token) return false;
    *v_idx = (int)val - 1; // Convert to 0-based indexing
    
    if (*end == '/') {
        end++; // Skip first '/'
        if (*end != '/') {
            // Has texture coordinate
            val = strtol(end, &end, 10);
            if (end != token && val != 0) {
                *vt_idx = (int)val - 1;
            }
        }
        
        if (*end == '/') {
            end++; // Skip second '/'
            val = strtol(end, &end, 10);
            if (end != token && val != 0) {
                *vn_idx = (int)val - 1;
            }
        }
    }
    
    return true;
}

void mioReadOBJ(
    const char* fpath,
    double** pVertices,
    double** pNormals,
    double** pTexCoords,
    unsigned int** pFaceSizes,
    unsigned int** pFaceVertexIndices,
    unsigned int** pFaceVertexTexCoordIndices,
    unsigned int** pFaceVertexNormalIndices,
    unsigned int* numVertices,
    unsigned int* numNormals,
    unsigned int* numTexcoords,
    unsigned int* numFaces)
{
    fprintf(stdout, "read .obj file: %s\n", fpath);
    
    // Initialize output parameters
    *pVertices = NULL;
    *pNormals = NULL;
    *pTexCoords = NULL;
    *pFaceSizes = NULL;
    *pFaceVertexIndices = NULL;
    *pFaceVertexTexCoordIndices = NULL;
    *pFaceVertexNormalIndices = NULL;
    *numVertices = 0;
    *numNormals = 0;
    *numTexcoords = 0;
    *numFaces = 0;
    
    FILE* file = fopen(fpath, "rb");
    if (file == NULL) {
        fprintf(stderr, "error: failed to open file '%s'\n", fpath);
        return;
    }
    
    char* lineBuf = NULL;
    size_t lineBufLen = 0;
    ParseCounts counts = {0, 0, 0, 0, 0};
    
    // First pass: count elements
    fprintf(stdout, "\tFirst pass: counting elements...\n");
    while (getline(&lineBuf, &lineBufLen, file) != (size_t)-1) {
        strip_trailing_whitespace(lineBuf);
        
        size_t lineLen = strlen(lineBuf);
        if (lineLen == 0 || lineBuf[0] == '#') {
            continue;
        }
        
        ObjCommandType cmdType = identify_command(lineBuf);
        switch (cmdType) {
            case CMD_VERTEX:
                counts.vertexCount++;
                break;
            case CMD_NORMAL:
                counts.normalCount++;
                break;
            case CMD_TEXCOORD:
                counts.texCoordCount++;
                break;
            case CMD_FACE:
                counts.faceCount++;
                counts.faceIndexCount += count_face_vertices(lineBuf);
                break;
            default:
                break;
        }
    }
    
    fprintf(stdout, "\t%d vertices\n", counts.vertexCount);
    fprintf(stdout, "\t%d normals\n", counts.normalCount);
    fprintf(stdout, "\t%d texture coordinates\n", counts.texCoordCount);
    fprintf(stdout, "\t%d faces\n", counts.faceCount);
    fprintf(stdout, "\t%d face indices\n", counts.faceIndexCount);
    
    // Allocate memory
    if (counts.vertexCount > 0) {
        *pVertices = (double*)safe_malloc(counts.vertexCount * 3 * sizeof(double));
    }
    if (counts.normalCount > 0) {
        *pNormals = (double*)safe_malloc(counts.normalCount * 3 * sizeof(double));
    }
    if (counts.texCoordCount > 0) {
        *pTexCoords = (double*)safe_malloc(counts.texCoordCount * 2 * sizeof(double));
    }
    if (counts.faceCount > 0) {
        *pFaceSizes = (unsigned int*)safe_malloc(counts.faceCount * sizeof(unsigned int));
    }
    if (counts.faceIndexCount > 0) {
        *pFaceVertexIndices = (unsigned int*)safe_malloc(counts.faceIndexCount * sizeof(unsigned int));
        if (counts.texCoordCount > 0) {
            *pFaceVertexTexCoordIndices = (unsigned int*)safe_malloc(counts.faceIndexCount * sizeof(unsigned int));
        }
        if (counts.normalCount > 0) {
            *pFaceVertexNormalIndices = (unsigned int*)safe_malloc(counts.faceIndexCount * sizeof(unsigned int));
        }
    }
    
    // Second pass: parse data
    fprintf(stdout, "\tSecond pass: parsing data...\n");
    rewind(file);
    
    int v_idx = 0, vn_idx = 0, vt_idx = 0, f_idx = 0, fi_idx = 0;
    
    while (getline(&lineBuf, &lineBufLen, file) != (size_t)-1) {
        strip_trailing_whitespace(lineBuf);
        
        size_t lineLen = strlen(lineBuf);
        if (lineLen == 0 || lineBuf[0] == '#') {
            continue;
        }
        
        ObjCommandType cmdType = identify_command(lineBuf);
        
        switch (cmdType) {
            case CMD_VERTEX: {
                double x, y, z;
                if (parse_vertex(lineBuf, &x, &y, &z)) {
                    (*pVertices)[v_idx * 3 + 0] = x;
                    (*pVertices)[v_idx * 3 + 1] = y;
                    (*pVertices)[v_idx * 3 + 2] = z;
                    v_idx++;
                } else {
                    fprintf(stderr, "warning: failed to parse vertex at index %d\n", v_idx);
                }
                break;
            }
            
            case CMD_NORMAL: {
                double x, y, z;
                if (parse_normal(lineBuf, &x, &y, &z)) {
                    (*pNormals)[vn_idx * 3 + 0] = x;
                    (*pNormals)[vn_idx * 3 + 1] = y;
                    (*pNormals)[vn_idx * 3 + 2] = z;
                    vn_idx++;
                } else {
                    fprintf(stderr, "warning: failed to parse normal at index %d\n", vn_idx);
                }
                break;
            }
            
            case CMD_TEXCOORD: {
                double u, v;
                if (parse_texcoord(lineBuf, &u, &v)) {
                    (*pTexCoords)[vt_idx * 2 + 0] = u;
                    (*pTexCoords)[vt_idx * 2 + 1] = v;
                    vt_idx++;
                } else {
                    fprintf(stderr, "warning: failed to parse texcoord at index %d\n", vt_idx);
                }
                break;
            }
            
            case CMD_FACE: {
                unsigned int faceVertCount = count_face_vertices(lineBuf);
                (*pFaceSizes)[f_idx] = faceVertCount;
                
                // Parse each vertex in the face
                const char* ptr = lineBuf + 2;
                char token[128];
                int token_idx = 0;
                
                for (unsigned int fv = 0; fv < faceVertCount; fv++) {
                    // Skip whitespace
                    while (*ptr == ' ' || *ptr == '\t') {
                        ptr++;
                    }
                    
                    // Extract token
                    token_idx = 0;
                    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && token_idx < 127) {
                        token[token_idx++] = *ptr++;
                    }
                    token[token_idx] = '\0';
                    
                    // Parse face vertex
                    int v, vt, vn;
                    if (parse_face_vertex(token, &v, &vt, &vn)) {
                        (*pFaceVertexIndices)[fi_idx] = (unsigned int)v;
                        if (vt >= 0 && *pFaceVertexTexCoordIndices != NULL) {
                            (*pFaceVertexTexCoordIndices)[fi_idx] = (unsigned int)vt;
                        }
                        if (vn >= 0 && *pFaceVertexNormalIndices != NULL) {
                            (*pFaceVertexNormalIndices)[fi_idx] = (unsigned int)vn;
                        }
                        fi_idx++;
                    }
                }
                
                f_idx++;
                break;
            }
            
            default:
                break;
        }
    }
    
    // Set output counts
    *numVertices = counts.vertexCount;
    *numNormals = counts.normalCount;
    *numTexcoords = counts.texCoordCount;
    *numFaces = counts.faceCount;
    
    // Cleanup
    free(lineBuf);
    fclose(file);
    
    fprintf(stdout, "done.\n");
}

void mioWriteOBJ(
    const char* fpath,
    double* pVertices,
    double* pNormals,
    double* pTexCoords,
    unsigned int* pFaceSizes,
    unsigned int* pFaceVertexIndices,
    unsigned int* pFaceVertexTexCoordIndices,
    unsigned int* pFaceVertexNormalIndices,
    unsigned int numVertices,
    unsigned int numNormals,
    unsigned int numTexcoords,
    unsigned int numFaces)
{
    fprintf(stdout, "write .obj file: %s\n", fpath);
    
    FILE* file = fopen(fpath, "w");
    if (file == NULL) {
        fprintf(stderr, "error: failed to open file '%s' for writing\n", fpath);
        return;
    }
    
    // Write vertices
    fprintf(stdout, "\twriting %u vertices\n", numVertices);
    for (unsigned int i = 0; i < numVertices; i++) {
        const double x = pVertices[i * 3 + 0];
        const double y = pVertices[i * 3 + 1];
        const double z = pVertices[i * 3 + 2];
        fprintf(file, "v %.6f %.6f %.6f\n", x, y, z);
    }
    
    // Write normals
    if (numNormals > 0) {
        fprintf(stdout, "\twriting %u normals\n", numNormals);
        for (unsigned int i = 0; i < numNormals; i++) {
            const double x = pNormals[i * 3 + 0];
            const double y = pNormals[i * 3 + 1];
            const double z = pNormals[i * 3 + 2];
            fprintf(file, "vn %.6f %.6f %.6f\n", x, y, z);
        }
    }
    
    // Write texture coordinates
    if (numTexcoords > 0) {
        fprintf(stdout, "\twriting %u texture coordinates\n", numTexcoords);
        for (unsigned int i = 0; i < numTexcoords; i++) {
            const double u = pTexCoords[i * 2 + 0];
            const double v = pTexCoords[i * 2 + 1];
            fprintf(file, "vt %.6f %.6f\n", u, v);
        }
    }
    
    // Write faces
    fprintf(stdout, "\twriting %u faces\n", numFaces);
    unsigned int baseIndex = 0;
    for (unsigned int f = 0; f < numFaces; f++) {
        const unsigned int faceSize = pFaceSizes[f];
        fprintf(file, "f");
        
        for (unsigned int v = 0; v < faceSize; v++) {
            const unsigned int vertexIdx = pFaceVertexIndices[baseIndex + v] + 1;
            
            if (numNormals > 0 && numTexcoords > 0) {
                const unsigned int normalIdx = pFaceVertexNormalIndices[baseIndex + v] + 1;
                const unsigned int texCoordIdx = pFaceVertexTexCoordIndices[baseIndex + v] + 1;
                fprintf(file, " %u/%u/%u", vertexIdx, texCoordIdx, normalIdx);
            } else if (numNormals > 0) {
                const unsigned int normalIdx = pFaceVertexNormalIndices[baseIndex + v] + 1;
                fprintf(file, " %u//%u", vertexIdx, normalIdx);
            } else if (numTexcoords > 0) {
                const unsigned int texCoordIdx = pFaceVertexTexCoordIndices[baseIndex + v] + 1;
                fprintf(file, " %u/%u", vertexIdx, texCoordIdx);
            } else {
                fprintf(file, " %u", vertexIdx);
            }
        }
        
        fprintf(file, "\n");
        baseIndex += faceSize;
    }
    
    fclose(file);
    fprintf(stdout, "done.\n");
}

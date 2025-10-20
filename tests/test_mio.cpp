/***************************************************************************
 * Copyright (C) 2023 CutDigital Enterprise Ltd
 * Licensed under the GNU GPL License, Version 3.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.gnu.org/licenses/gpl-3.0.html.
 *
 * Comprehensive Google Test suite for MIO library file parsers
 *
 * Author(s): Floyd M. Chitalu CutDigital Enterprise Ltd.
 **************************************************************************/

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <cmath>
#include "mio/obj.h"
#include "mio/off.h"
#include "mio/stl.h"

namespace fs = std::filesystem;

// ============================================================================
// Test Fixture Base Class
// ============================================================================

class MeshFileTest : public ::testing::Test {
protected:
    fs::path tempDir;
    
    void SetUp() override {
        // Create temporary directory for test files
        tempDir = fs::temp_directory_path() / "mio_test";
        fs::create_directories(tempDir);
    }
    
    void TearDown() override {
        // Clean up temporary directory
        if (fs::exists(tempDir)) {
            fs::remove_all(tempDir);
        }
    }
    
    // Helper to create temporary file path
    fs::path getTempPath(const std::string& filename) {
        return tempDir / filename;
    }
    
    // Helper to write string to file
    void writeFile(const fs::path& path, const std::string& content) {
        std::ofstream file(path);
        ASSERT_TRUE(file.is_open()) << "Failed to create test file: " << path;
        file << content;
        file.close();
    }
    
    // Helper to compare floating point arrays with tolerance
    void expectArrayNear(const double* actual, const double* expected, 
                         unsigned int count, double tolerance = 1e-6) {
        for (unsigned int i = 0; i < count; ++i) {
            EXPECT_NEAR(actual[i], expected[i], tolerance) 
                << "Mismatch at index " << i;
        }
    }
};

// ============================================================================
// OBJ File Tests
// ============================================================================

class ObjFileTest : public MeshFileTest {};

TEST_F(ObjFileTest, ReadSimpleTriangle) {
    const std::string objContent = R"(
# Simple triangle
v 0.0 0.0 0.0
v 1.0 0.0 0.0
v 0.5 1.0 0.0
f 1 2 3
)";
    
    auto filepath = getTempPath("triangle.obj");
    writeFile(filepath, objContent);
    
    double* vertices = nullptr;
    double* normals = nullptr;
    double* texCoords = nullptr;
    unsigned int* faceSizes = nullptr;
    unsigned int* faceVertexIndices = nullptr;
    unsigned int* faceVertexTexCoordIndices = nullptr;
    unsigned int* faceVertexNormalIndices = nullptr;
    unsigned int numVertices = 0;
    unsigned int numNormals = 0;
    unsigned int numTexcoords = 0;
    unsigned int numFaces = 0;
    
    mioReadOBJ(filepath.string().c_str(),
               &vertices, &normals, &texCoords,
               &faceSizes, &faceVertexIndices,
               &faceVertexTexCoordIndices, &faceVertexNormalIndices,
               &numVertices, &numNormals, &numTexcoords, &numFaces);
    
    ASSERT_NE(vertices, nullptr);
    EXPECT_EQ(numVertices, 3);
    EXPECT_EQ(numFaces, 1);
    EXPECT_EQ(faceSizes[0], 3);
    
    // Check vertex coordinates
    EXPECT_DOUBLE_EQ(vertices[0], 0.0);
    EXPECT_DOUBLE_EQ(vertices[1], 0.0);
    EXPECT_DOUBLE_EQ(vertices[2], 0.0);
    EXPECT_DOUBLE_EQ(vertices[3], 1.0);
    
    // Check face indices
    EXPECT_EQ(faceVertexIndices[0], 0);
    EXPECT_EQ(faceVertexIndices[1], 1);
    EXPECT_EQ(faceVertexIndices[2], 2);
    
    free(vertices);
    free(normals);
    free(texCoords);
    free(faceSizes);
    free(faceVertexIndices);
    free(faceVertexTexCoordIndices);
    free(faceVertexNormalIndices);
}

TEST_F(ObjFileTest, ReadWithNormalsAndTexCoords) {
    const std::string objContent = R"(
v 0.0 0.0 0.0
v 1.0 0.0 0.0
v 0.5 1.0 0.0
vn 0.0 0.0 1.0
vn 0.0 0.0 1.0
vn 0.0 0.0 1.0
vt 0.0 0.0
vt 1.0 0.0
vt 0.5 1.0
f 1/1/1 2/2/2 3/3/3
)";
    
    auto filepath = getTempPath("with_normals.obj");
    writeFile(filepath, objContent);
    
    double* vertices = nullptr;
    double* normals = nullptr;
    double* texCoords = nullptr;
    unsigned int* faceSizes = nullptr;
    unsigned int* faceVertexIndices = nullptr;
    unsigned int* faceVertexTexCoordIndices = nullptr;
    unsigned int* faceVertexNormalIndices = nullptr;
    unsigned int numVertices = 0;
    unsigned int numNormals = 0;
    unsigned int numTexcoords = 0;
    unsigned int numFaces = 0;
    
    mioReadOBJ(filepath.string().c_str(),
               &vertices, &normals, &texCoords,
               &faceSizes, &faceVertexIndices,
               &faceVertexTexCoordIndices, &faceVertexNormalIndices,
               &numVertices, &numNormals, &numTexcoords, &numFaces);
    
    EXPECT_EQ(numVertices, 3);
    EXPECT_EQ(numNormals, 3);
    EXPECT_EQ(numTexcoords, 3);
    EXPECT_EQ(numFaces, 1);
    
    ASSERT_NE(normals, nullptr);
    EXPECT_DOUBLE_EQ(normals[2], 1.0); // Z component of first normal
    
    ASSERT_NE(texCoords, nullptr);
    EXPECT_DOUBLE_EQ(texCoords[0], 0.0);
    EXPECT_DOUBLE_EQ(texCoords[1], 0.0);
    
    free(vertices);
    free(normals);
    free(texCoords);
    free(faceSizes);
    free(faceVertexIndices);
    free(faceVertexTexCoordIndices);
    free(faceVertexNormalIndices);
}

TEST_F(ObjFileTest, ReadQuadFace) {
    const std::string objContent = R"(
v 0.0 0.0 0.0
v 1.0 0.0 0.0
v 1.0 1.0 0.0
v 0.0 1.0 0.0
f 1 2 3 4
)";
    
    auto filepath = getTempPath("quad.obj");
    writeFile(filepath, objContent);
    
    double* vertices = nullptr;
    double* normals = nullptr;
    double* texCoords = nullptr;
    unsigned int* faceSizes = nullptr;
    unsigned int* faceVertexIndices = nullptr;
    unsigned int* faceVertexTexCoordIndices = nullptr;
    unsigned int* faceVertexNormalIndices = nullptr;
    unsigned int numVertices = 0;
    unsigned int numNormals = 0;
    unsigned int numTexcoords = 0;
    unsigned int numFaces = 0;
    
    mioReadOBJ(filepath.string().c_str(),
               &vertices, &normals, &texCoords,
               &faceSizes, &faceVertexIndices,
               &faceVertexTexCoordIndices, &faceVertexNormalIndices,
               &numVertices, &numNormals, &numTexcoords, &numFaces);
    
    EXPECT_EQ(numFaces, 1);
    EXPECT_EQ(faceSizes[0], 4);
    
    free(vertices);
    free(normals);
    free(texCoords);
    free(faceSizes);
    free(faceVertexIndices);
    free(faceVertexTexCoordIndices);
    free(faceVertexNormalIndices);
}

TEST_F(ObjFileTest, WriteAndReadRoundTrip) {
    // Create test data
    double vertices[] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.5, 1.0, 0.0};
    unsigned int faceSizes[] = {3};
    unsigned int faceVertexIndices[] = {0, 1, 2};
    
    auto filepath = getTempPath("roundtrip.obj");
    
    // Write
    mioWriteOBJ(filepath.string().c_str(),
                vertices, nullptr, nullptr,
                faceSizes, faceVertexIndices,
                nullptr, nullptr,
                3, 0, 0, 1);
    
    // Read back
    double* readVertices = nullptr;
    double* readNormals = nullptr;
    double* readTexCoords = nullptr;
    unsigned int* readFaceSizes = nullptr;
    unsigned int* readFaceVertexIndices = nullptr;
    unsigned int* readFaceVertexTexCoordIndices = nullptr;
    unsigned int* readFaceVertexNormalIndices = nullptr;
    unsigned int numVertices = 0;
    unsigned int numNormals = 0;
    unsigned int numTexcoords = 0;
    unsigned int numFaces = 0;
    
    mioReadOBJ(filepath.string().c_str(),
               &readVertices, &readNormals, &readTexCoords,
               &readFaceSizes, &readFaceVertexIndices,
               &readFaceVertexTexCoordIndices, &readFaceVertexNormalIndices,
               &numVertices, &numNormals, &numTexcoords, &numFaces);
    
    EXPECT_EQ(numVertices, 3);
    EXPECT_EQ(numFaces, 1);
    expectArrayNear(readVertices, vertices, 9);
    
    free(readVertices);
    free(readNormals);
    free(readTexCoords);
    free(readFaceSizes);
    free(readFaceVertexIndices);
    free(readFaceVertexTexCoordIndices);
    free(readFaceVertexNormalIndices);
}

TEST_F(ObjFileTest, HandleEmptyFile) {
    auto filepath = getTempPath("empty.obj");
    writeFile(filepath, "");
    
    double* vertices = nullptr;
    double* normals = nullptr;
    double* texCoords = nullptr;
    unsigned int* faceSizes = nullptr;
    unsigned int* faceVertexIndices = nullptr;
    unsigned int* faceVertexTexCoordIndices = nullptr;
    unsigned int* faceVertexNormalIndices = nullptr;
    unsigned int numVertices = 0;
    unsigned int numNormals = 0;
    unsigned int numTexcoords = 0;
    unsigned int numFaces = 0;
    
    mioReadOBJ(filepath.string().c_str(),
               &vertices, &normals, &texCoords,
               &faceSizes, &faceVertexIndices,
               &faceVertexTexCoordIndices, &faceVertexNormalIndices,
               &numVertices, &numNormals, &numTexcoords, &numFaces);
    
    EXPECT_EQ(numVertices, 0);
    EXPECT_EQ(numFaces, 0);
    
    free(vertices);
    free(normals);
    free(texCoords);
    free(faceSizes);
    free(faceVertexIndices);
    free(faceVertexTexCoordIndices);
    free(faceVertexNormalIndices);
}

// ============================================================================
// OFF File Tests
// ============================================================================

class OffFileTest : public MeshFileTest {};

TEST_F(OffFileTest, ReadSimpleTetrahedron) {
    const std::string offContent = R"(OFF
4 4 6
0.0 0.0 0.0
1.0 0.0 0.0
0.5 1.0 0.0
0.5 0.5 1.0
3 0 1 2
3 0 1 3
3 1 2 3
3 2 0 3
)";
    
    auto filepath = getTempPath("tetrahedron.off");
    writeFile(filepath, offContent);
    
    double* vertices = nullptr;
    unsigned int* faceVertexIndices = nullptr;
    unsigned int* faceSizes = nullptr;
    unsigned int numVertices = 0;
    unsigned int numFaces = 0;
    
    mioReadOFF(filepath.string().c_str(),
               &vertices, &faceVertexIndices, &faceSizes,
               &numVertices, &numFaces);
    
    ASSERT_NE(vertices, nullptr);
    EXPECT_EQ(numVertices, 4);
    EXPECT_EQ(numFaces, 4);
    
    // Check first vertex
    EXPECT_DOUBLE_EQ(vertices[0], 0.0);
    EXPECT_DOUBLE_EQ(vertices[1], 0.0);
    EXPECT_DOUBLE_EQ(vertices[2], 0.0);
    
    // Check all faces are triangles
    for (unsigned int i = 0; i < numFaces; ++i) {
        EXPECT_EQ(faceSizes[i], 3) << "Face " << i << " should have 3 vertices";
    }
    
    free(vertices);
    free(faceVertexIndices);
    free(faceSizes);
}

TEST_F(OffFileTest, ReadWithComments) {
    const std::string offContent = R"(OFF
# This is a comment
3 1 0
# Vertices
0.0 0.0 0.0
1.0 0.0 0.0
0.5 1.0 0.0
# Face
3 0 1 2
)";
    
    auto filepath = getTempPath("with_comments.off");
    writeFile(filepath, offContent);
    
    double* vertices = nullptr;
    unsigned int* faceVertexIndices = nullptr;
    unsigned int* faceSizes = nullptr;
    unsigned int numVertices = 0;
    unsigned int numFaces = 0;
    
    mioReadOFF(filepath.string().c_str(),
               &vertices, &faceVertexIndices, &faceSizes,
               &numVertices, &numFaces);
    
    EXPECT_EQ(numVertices, 3);
    EXPECT_EQ(numFaces, 1);
    
    free(vertices);
    free(faceVertexIndices);
    free(faceSizes);
}

TEST_F(OffFileTest, WriteAndReadRoundTrip) {
    double vertices[] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.5, 1.0, 0.0};
    unsigned int faceSizes[] = {3};
    unsigned int faceVertexIndices[] = {0, 1, 2};
    
    auto filepath = getTempPath("roundtrip.off");
    
    // Write
    mioWriteOFF(filepath.string().c_str(),
                vertices, faceVertexIndices, faceSizes, nullptr,
                3, 1, 0);
    
    // Read back
    double* readVertices = nullptr;
    unsigned int* readFaceVertexIndices = nullptr;
    unsigned int* readFaceSizes = nullptr;
    unsigned int numVertices = 0;
    unsigned int numFaces = 0;
    
    mioReadOFF(filepath.string().c_str(),
               &readVertices, &readFaceVertexIndices, &readFaceSizes,
               &numVertices, &numFaces);
    
    EXPECT_EQ(numVertices, 3);
    EXPECT_EQ(numFaces, 1);
    expectArrayNear(readVertices, vertices, 9);
    
    free(readVertices);
    free(readFaceVertexIndices);
    free(readFaceSizes);
}

// ============================================================================
// STL File Tests
// ============================================================================

class StlFileTest : public MeshFileTest {};

TEST_F(StlFileTest, WriteAndReadASCII) {
    // Single triangle with normal
    double vertices[] = {
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.5, 1.0, 0.0
    };
    double normals[] = {0.0, 0.0, 1.0};
    
    auto filepath = getTempPath("triangle.stl");
    
    // Write
    mioWriteSTL(filepath.string().c_str(), vertices, normals, 3);
    
    // Read back
    double* readVertices = nullptr;
    double* readNormals = nullptr;
    unsigned int numVertices = 0;
    
    mioReadSTL(filepath.string().c_str(),
               &readVertices, &readNormals, &numVertices);
    
    EXPECT_EQ(numVertices, 3);
    expectArrayNear(readVertices, vertices, 9);
    expectArrayNear(readNormals, normals, 3);
    
    free(readVertices);
    free(readNormals);
}

TEST_F(StlFileTest, ReadASCIIMultipleTriangles) {
    const std::string stlContent = R"(solid TestMesh
  facet normal 0.0 0.0 1.0
    outer loop
      vertex 0.0 0.0 0.0
      vertex 1.0 0.0 0.0
      vertex 0.5 1.0 0.0
    endloop
  endfacet
  facet normal 0.0 0.0 1.0
    outer loop
      vertex 1.0 0.0 0.0
      vertex 1.5 1.0 0.0
      vertex 0.5 1.0 0.0
    endloop
  endfacet
endsolid TestMesh
)";
    
    auto filepath = getTempPath("multi_triangle.stl");
    writeFile(filepath, stlContent);
    
    double* vertices = nullptr;
    double* normals = nullptr;
    unsigned int numVertices = 0;
    
    mioReadSTL(filepath.string().c_str(),
               &vertices, &normals, &numVertices);
    
    EXPECT_EQ(numVertices, 6); // 2 triangles = 6 vertices
    ASSERT_NE(vertices, nullptr);
    ASSERT_NE(normals, nullptr);
    
    // Check first vertex
    EXPECT_DOUBLE_EQ(vertices[0], 0.0);
    EXPECT_DOUBLE_EQ(vertices[1], 0.0);
    EXPECT_DOUBLE_EQ(vertices[2], 0.0);
    
    // Check normals
    EXPECT_DOUBLE_EQ(normals[2], 1.0); // Z component of first normal
    EXPECT_DOUBLE_EQ(normals[5], 1.0); // Z component of second normal
    
    free(vertices);
    free(normals);
}

TEST_F(StlFileTest, HandleInvalidVertexCount) {
    // Test with vertex count not divisible by 3
    double vertices[] = {0.0, 0.0, 0.0, 1.0, 0.0};
    double normals[] = {0.0, 0.0, 1.0};
    
    auto filepath = getTempPath("invalid.stl");
    
    // Should handle gracefully (function will warn but not crash)
    testing::internal::CaptureStderr();
    mioWriteSTL(filepath.string().c_str(), vertices, normals, 2);
    std::string output = testing::internal::GetCapturedStderr();
    
    EXPECT_TRUE(output.find("warning") != std::string::npos);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

class ErrorHandlingTest : public MeshFileTest {};

TEST_F(ErrorHandlingTest, ObjNonexistentFile) {
    double* vertices = nullptr;
    double* normals = nullptr;
    double* texCoords = nullptr;
    unsigned int* faceSizes = nullptr;
    unsigned int* faceVertexIndices = nullptr;
    unsigned int* faceVertexTexCoordIndices = nullptr;
    unsigned int* faceVertexNormalIndices = nullptr;
    unsigned int numVertices = 0;
    unsigned int numNormals = 0;
    unsigned int numTexcoords = 0;
    unsigned int numFaces = 0;
    
    mioReadOBJ("/nonexistent/path/file.obj",
               &vertices, &normals, &texCoords,
               &faceSizes, &faceVertexIndices,
               &faceVertexTexCoordIndices, &faceVertexNormalIndices,
               &numVertices, &numNormals, &numTexcoords, &numFaces);
    
    // Should fail gracefully
    EXPECT_EQ(numVertices, 0);
    EXPECT_EQ(vertices, nullptr);
}

TEST_F(ErrorHandlingTest, OffInvalidHeader) {
    const std::string offContent = "NOTOFF\n3 1 0\n";
    
    auto filepath = getTempPath("invalid_header.off");
    writeFile(filepath, offContent);
    
    double* vertices = nullptr;
    unsigned int* faceVertexIndices = nullptr;
    unsigned int* faceSizes = nullptr;
    unsigned int numVertices = 0;
    unsigned int numFaces = 0;
    
    testing::internal::CaptureStderr();
    mioReadOFF(filepath.string().c_str(),
               &vertices, &faceVertexIndices, &faceSizes,
               &numVertices, &numFaces);
    std::string output = testing::internal::GetCapturedStderr();
    
    EXPECT_EQ(numVertices, 0);
    EXPECT_TRUE(output.find("error") != std::string::npos);
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

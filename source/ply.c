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
 * ply.c 
 *
 * \brief: Robust PLY file parser with improved error handling and memory safety
 *
 * Author(s): Floyd M. Chitalu CutDigital Enterprise Ltd.
 **************************************************************************/

#include "mio/ply.h"
#include "mio/third-party/ply.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* User's vertex and face definitions for a polygonal object */
typedef struct Vertex
{
	float x, y, z; /* 3D position */
} Vertex;

typedef struct Face
{
	unsigned char nverts; /* number of vertex indices in list */
	int* verts; /* vertex index list */
} Face;

/* Information needed to describe the user's data to the PLY routines */
static const char* elem_names[] = {"vertex", "face"};

static PlyProperty vert_props[] = {
	{"x", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex, x), 0, 0, 0, 0},
	{"y", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex, y), 0, 0, 0, 0},
	{"z", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex, z), 0, 0, 0, 0},
};

static PlyProperty face_props[] = {
	{"vertex_indices",
	 PLY_INT,
	 PLY_INT,
	 offsetof(Face, verts),
	 1,
	 PLY_UCHAR,
	 PLY_UCHAR,
	 offsetof(Face, nverts)},
};

/* Safe memory allocation with error checking */
static void* safe_malloc(size_t size)
{
	if(size == 0)
	{
		return NULL;
	}
	void* ptr = malloc(size);
	if(ptr == NULL)
	{
		fprintf(stderr, "error: memory allocation of %zu bytes failed\n", size);
		exit(EXIT_FAILURE);
	}
	return ptr;
}

static void* safe_realloc(void* ptr, size_t size)
{
	if(size == 0)
	{
		free(ptr);
		return NULL;
	}
	void* new_ptr = realloc(ptr, size);
	if(new_ptr == NULL)
	{
		fprintf(stderr, "error: memory reallocation to %zu bytes failed\n", size);
		free(ptr);
		exit(EXIT_FAILURE);
	}
	return new_ptr;
}

void mioReadPLY(const char* fpath,
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
	fprintf(stdout, "read .ply file: %s\n", fpath);

	/* Initialize all output parameters */
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

	/* Open PLY file for reading */
	int nelems = 0;
	char** elist = NULL;
	int file_type = 0;
	float version = 0.0f;

	PlyFile* ply = ply_open_for_reading(fpath, &nelems, &elist, &file_type, &version);
	if(ply == NULL)
	{
		fprintf(stderr, "error: failed to open '%s'\n", fpath);
		return;
	}

	fprintf(stdout, "\tversion %.1f\n", version);
	fprintf(stdout, "\ttype %d\n", file_type);

	Face** flist = NULL;

	/* Process each element type in the file */
	for(int i = 0; i < nelems; i++)
	{
		char* elem_name = elist[i];
		int num_elems = 0;
		int nprops = 0;

		PlyProperty** plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);

		fprintf(stdout, "\telement %s: %d\n", elem_name, num_elems);

		/* Process vertex elements */
		if(equal_strings("vertex", elem_name))
		{
			if(num_elems <= 0)
			{
				fprintf(stderr, "warning: no vertices in file\n");
				continue;
			}

			*numVertices = (unsigned int)num_elems;
			*pVertices = (double*)safe_malloc(num_elems * sizeof(double) * 3);

			/* Set up property reading */
			ply_get_property(ply, elem_name, &vert_props[0]);
			ply_get_property(ply, elem_name, &vert_props[1]);
			ply_get_property(ply, elem_name, &vert_props[2]);

			/* Read all vertex elements */
			for(int j = 0; j < num_elems; j++)
			{
				Vertex vert;
				ply_get_element(ply, (void*)&vert);

				(*pVertices)[j * 3 + 0] = (double)vert.x;
				(*pVertices)[j * 3 + 1] = (double)vert.y;
				(*pVertices)[j * 3 + 2] = (double)vert.z;
			}
		}
		/* Process face elements */
		else if(equal_strings("face", elem_name))
		{
			if(num_elems <= 0)
			{
				fprintf(stderr, "warning: no faces in file\n");
				continue;
			}

			*numFaces = (unsigned int)num_elems;
			flist = (Face**)safe_malloc(sizeof(Face*) * num_elems);
			*pFaceSizes = (unsigned int*)safe_malloc(sizeof(unsigned int) * num_elems);

			/* Set up property reading */
			ply_get_property(ply, elem_name, &face_props[0]);

			/* First pass: read faces and count total indices */
			unsigned int faceIndicesCount = 0;
			for(int j = 0; j < num_elems; j++)
			{
				flist[j] = (Face*)safe_malloc(sizeof(Face));
				flist[j]->verts = NULL;

				ply_get_element(ply, (void*)flist[j]);

				if(flist[j]->nverts < 3)
				{
					fprintf(stderr,
							"warning: face %d has only %d vertices (minimum 3)\n",
							j,
							flist[j]->nverts);
				}

				(*pFaceSizes)[j] = flist[j]->nverts;
				faceIndicesCount += flist[j]->nverts;
			}

			fprintf(stdout, "\t%u total face indices\n", faceIndicesCount);

			/* Allocate face vertex indices array */
			*pFaceVertexIndices =
				(unsigned int*)safe_malloc(sizeof(unsigned int) * faceIndicesCount);

			/* Second pass: copy indices to output array */
			unsigned int base = 0;
			for(int j = 0; j < num_elems; j++)
			{
				const Face* const face = flist[j];

				for(int k = 0; k < face->nverts; k++)
				{
					int vertex_idx = face->verts[k];

					/* Validate index */
					if(vertex_idx < 0 || (unsigned int)vertex_idx >= *numVertices)
					{
						fprintf(stderr,
								"warning: face %d has invalid vertex index %d\n",
								j,
								vertex_idx);
						vertex_idx = 0; /* Clamp to valid range */
					}

					(*pFaceVertexIndices)[base + k] = (unsigned int)vertex_idx;
				}

				base += face->nverts;
			}
		}

		/* Print properties for debugging */
		if(plist != NULL)
		{
			for(int j = 0; j < nprops; j++)
			{
				if(plist[j] != NULL)
				{
					fprintf(stdout, "\t\tproperty: %s\n", plist[j]->name);
				}
			}
		}
	}

	/* Read and display comments */
	int num_comments = 0;
	char** comments = ply_get_comments(ply, &num_comments);
	for(int i = 0; i < num_comments; i++)
	{
		fprintf(stdout, "\tcomment: '%s'\n", comments[i]);
	}

	/* Read and display object info */
	int num_obj_info = 0;
	char** obj_info = ply_get_obj_info(ply, &num_obj_info);
	for(int i = 0; i < num_obj_info; i++)
	{
		fprintf(stdout, "\tobj_info: '%s'\n", obj_info[i]);
	}

	/* Cleanup temporary face list */
	if(flist != NULL)
	{
		for(unsigned int j = 0; j < *numFaces; j++)
		{
			if(flist[j] != NULL)
			{
				free(flist[j]->verts);
				free(flist[j]);
			}
		}
		free(flist);
	}

	/* Close the PLY file */
	ply_close(ply);

	fprintf(stdout, "done.\n");
}

void mioWritePLY(const char* fpath,
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
	fprintf(stdout, "write .ply file: %s\n", fpath);

	if(pVertices == NULL || numVertices == 0)
	{
		fprintf(stderr, "error: no vertices to write\n");
		return;
	}

	if(pFaceVertexIndices == NULL || pFaceSizes == NULL || numFaces == 0)
	{
		fprintf(stderr, "error: no faces to write\n");
		return;
	}

	/* Open PLY file for writing (ASCII format) */
	float version = 0.0f;
	PlyFile* ply = ply_open_for_writing(fpath, 2, (char**)elem_names, PLY_ASCII, &version);

	if(ply == NULL)
	{
		fprintf(stderr, "error: failed to open '%s' for writing\n", fpath);
		return;
	}

	/* Describe vertex element properties */
	ply_element_count(ply, "vertex", (int)numVertices);
	ply_describe_property(ply, "vertex", &vert_props[0]);
	ply_describe_property(ply, "vertex", &vert_props[1]);
	ply_describe_property(ply, "vertex", &vert_props[2]);

	/* Describe face element properties */
	ply_element_count(ply, "face", (int)numFaces);
	ply_describe_property(ply, "face", &face_props[0]);

	/* Add metadata */
	ply_put_comment(ply, "author: Floyd M. Chitalu");
	ply_put_obj_info(ply, "Generated by mio library");

	/* Complete the header */
	ply_header_complete(ply);

	/* Write vertex elements */
	fprintf(stdout, "\twriting %u vertices\n", numVertices);
	ply_put_element_setup(ply, "vertex");

	for(unsigned int i = 0; i < numVertices; i++)
	{
		Vertex vert;
		vert.x = (float)pVertices[i * 3 + 0];
		vert.y = (float)pVertices[i * 3 + 1];
		vert.z = (float)pVertices[i * 3 + 2];
		ply_put_element(ply, (void*)&vert);
	}

	/* Write face elements */
	fprintf(stdout, "\twriting %u faces\n", numFaces);
	ply_put_element_setup(ply, "face");

	unsigned int base = 0;
	int* tmp_indices = NULL;
	size_t tmp_size = 0;

	for(unsigned int i = 0; i < numFaces; i++)
	{
		Face face;
		face.nverts = (unsigned char)pFaceSizes[i];

		/* Reallocate temporary buffer if needed */
		if(face.nverts > tmp_size)
		{
			tmp_indices = (int*)safe_realloc(tmp_indices, sizeof(int) * face.nverts);
			tmp_size = face.nverts;
		}

		/* Copy indices to temporary buffer */
		for(unsigned int j = 0; j < face.nverts; j++)
		{
			unsigned int idx = pFaceVertexIndices[base + j];

			/* Validate index */
			if(idx >= numVertices)
			{
				fprintf(stderr,
						"warning: face %u has invalid vertex index %u (max: %u)\n",
						i,
						idx,
						numVertices - 1);
				idx = 0; /* Clamp to valid range */
			}

			tmp_indices[j] = (int)idx;
		}

		base += face.nverts;
		face.verts = tmp_indices;

		ply_put_element(ply, (void*)&face);
	}

	/* Cleanup */
	free(tmp_indices);

	/* Close the PLY file */
	ply_close(ply);

	fprintf(stdout, "done.\n");
}

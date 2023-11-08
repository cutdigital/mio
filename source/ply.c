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
 * ply.c
 *
 * \brief:
 *  TODO: ...
 *
 * Author(s):
 *
 *    Floyd M. Chitalu    CutDigital Enterprise Ltd.
 *
 **************************************************************************/

#include "mio/ply.h"
#include "mio/third-party/ply.h"

#include <assert.h>
#include <stdlib.h>

/* user's vertex and face definitions for a polygonal object */

typedef struct Vertex
{
	float x, y, z; /* the usual 3-space position of a vertex */
} Vertex;

typedef struct Face
{
	//unsigned char intensity; /* this user attaches intensity to faces */
	unsigned char nverts; /* number of vertex indices in list */
	int* verts; /* vertex index list */
} Face;

/* information needed to describe the user's data to the PLY routines */

char* elem_names[] = {/* list of the kinds of elements in the user's object */
					  "vertex",
					  "face"};

PlyProperty vert_props[] = {
	/* list of property information for a vertex */
	{"x", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex, x), 0, 0, 0, 0},
	{"y", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex, y), 0, 0, 0, 0},
	{"z", PLY_FLOAT, PLY_FLOAT, offsetof(Vertex, z), 0, 0, 0, 0},
};

PlyProperty face_props[] = {
	/* list of property information for a vertex */
	//{"intensity", PLY_UCHAR, PLY_UCHAR, offsetof(Face,intensity), 0, 0, 0, 0},
	{"vertex_indices",
	 PLY_INT,
	 PLY_INT,
	 offsetof(Face, verts),
	 1,
	 PLY_UCHAR,
	 PLY_UCHAR,
	 offsetof(Face, nverts)},
};

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
	fprintf(stdout, "read .ply file: %s\n", fpath);
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

	int i, j, k;
	PlyFile* ply;
	int nelems;
	char** elist;
	int file_type;
	float version;
	int nprops;
	int num_elems;
	PlyProperty** plist;
	Vertex** vlist;
	Face** flist;
	char* elem_name;
	int num_comments;
	char** comments;
	int num_obj_info;
	char** obj_info;

	/* open a PLY file for reading */
	ply = ply_open_for_reading(fpath, &nelems, &elist, &file_type, &version);

	/* print what we found out about the file */
	printf("version %f\n", version);
	printf("type %d\n", file_type);

	/* go through each kind of element that we learned is in the file */
	/* and read them */
	for(i = 0; i < nelems; i++)
	{

		/* get the description of the first element */
		elem_name = elist[i];
		plist = ply_get_element_description(ply, elem_name, &num_elems, &nprops);

		/* print the name of the element, for debugging */
		printf("element %s %d\n", elem_name, num_elems);

		/* if we're on vertex elements, read them in */
		if(equal_strings("vertex", elem_name))
		{
			*numVertices = num_elems;

			/* create a vertex list to hold all the vertices */
			//vlist = (Vertex**)malloc(sizeof(Vertex*) * num_elems);
			*pVertices = (double*)malloc(num_elems * sizeof(double) * 3);

			/* set up for getting vertex elements */

			ply_get_property(ply, elem_name, &vert_props[0]);
			ply_get_property(ply, elem_name, &vert_props[1]);
			ply_get_property(ply, elem_name, &vert_props[2]);

			/* grab all the vertex elements */
			for(j = 0; j < num_elems; j++)
			{

				/* grab and element from the file */
				//vlist[j] = (Vertex*)malloc(sizeof(Vertex));
				//ply_get_element(ply, (void*)vlist[j]);

				Vertex vert;
				ply_get_element(ply, (void*)&vert);

				(*pVertices)[j * 3 + 0] = vert.x;
				(*pVertices)[j * 3 + 1] = vert.y;
				(*pVertices)[j * 3 + 2] = vert.z;

				/* print out vertex x,y,z for debugging */
				//printf("vertex: %g %g %g\n", vlist[j]->x, vlist[j]->y, vlist[j]->z);
			}
		}

		/* if we're on face elements, read them in */
		if(equal_strings("face", elem_name))
		{

			/* create a list to hold all the face elements */
			flist = (Face**)malloc(sizeof(Face*) * num_elems);

			*pFaceSizes = (unsigned int*)malloc(sizeof(unsigned int) * num_elems);
			*numFaces = num_elems;

			/* set up for getting face elements */

			ply_get_property(ply, elem_name, &face_props[0]);
			//ply_get_property(ply, elem_name, &face_props[1]);

			unsigned int faceIndicesCount = 0;
			/* grab all the face elements */
			for(j = 0; j < num_elems; j++)
			{
				/* grab and element from the file */
				flist[j] = (Face*)malloc(sizeof(Face));
				ply_get_element(ply, (void*)flist[j]);

				/* print out face info, for debugging */
				//printf("face: %d, list = ", flist[j]->intensity);

				(*pFaceSizes)[j] = flist[j]->nverts;

				faceIndicesCount += flist[j]->nverts;

				//for(k = 0; k < flist[j]->nverts; k++)
				//	printf("%d ", flist[j]->verts[k]);
				//printf("\n");
			}

			// now allocate user pointer and copy data
			*pFaceVertexIndices = (unsigned int*)malloc(sizeof(unsigned int) * faceIndicesCount);
			// fill
			unsigned int base = 0;

			for(j = 0; j < num_elems; j++) // for each face
			{
				const Face* const face = flist[j];
				for(k = 0; k < flist[j]->nverts; k++) // for each vertex in face
				{
					(*pFaceVertexIndices)[base + k] = face->verts[k];
				}
				base += flist[j]->nverts;
			}
		}

		/* print out the properties we got, for debugging */
		for(j = 0; j < nprops; j++)
			printf("property %s\n", plist[j]->name);
	}

	/* grab and print out the comments in the file */
	comments = ply_get_comments(ply, &num_comments);
	for(i = 0; i < num_comments; i++)
		printf("comment = '%s'\n", comments[i]);

	/* grab and print out the object information */
	obj_info = ply_get_obj_info(ply, &num_obj_info);
	for(i = 0; i < num_obj_info; i++)
		printf("obj_info = '%s'\n", obj_info[i]);

	/* close the PLY file */
	ply_close(ply);
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
	unsigned int numFaces)
{
	fprintf(stdout, "write .ply file: %s\n", fpath);

	int i, j;
	PlyFile* ply;
	int nelems;
	char** elist;
	int file_type;
	float version;
	int nverts = numVertices; //sizeof (verts) / sizeof (Vertex);
	int nfaces = numFaces; //sizeof (faces) / sizeof (Face);

	/* create the vertex index lists for the faces */
	//for(i = 0; i < nfaces; i++)
	//	faces[i].verts = vert_ptrs[i];

	/* open either a binary or ascii PLY file for writing */

#if 1
	ply = ply_open_for_writing(fpath, 2, elem_names, PLY_ASCII, &version);
#else
	ply = ply_open_for_writing(fpath, 2, elem_names, PLY_BINARY_BE, &version);
#endif

	/* describe what properties go into the vertex and face elements */

	ply_element_count(ply, "vertex", nverts);
	ply_describe_property(ply, "vertex", &vert_props[0]);
	ply_describe_property(ply, "vertex", &vert_props[1]);
	ply_describe_property(ply, "vertex", &vert_props[2]);

	ply_element_count(ply, "face", nfaces);
	ply_describe_property(ply, "face", &face_props[0]);
	//ply_describe_property (ply, "face", &face_props[1]);

	/* write a comment and an object information field */
	ply_put_comment(ply, "author: Floyd M. Chitalu");
	ply_put_obj_info(ply, "blah..");

	/* we have described exactly what we will put in the file, so */
	/* we are now done with the header info */
	ply_header_complete(ply);

	/* set up and write the vertex elements */
	ply_put_element_setup(ply, "vertex");

	for(i = 0; i < nverts; i++)
	{
		Vertex vert;
		vert.x = pVertices[i * 3 + 0];
		vert.y = pVertices[i * 3 + 1];
		vert.z = pVertices[i * 3 + 2];

		ply_put_element(ply, (void*)&vert);
	}

	/* set up and write the face elements */
	ply_put_element_setup(ply, "face");

	unsigned int base = 0;
	int* tmp_ptr = NULL;

	for(i = 0; i < nfaces; i++)
	{
		Face face;
		face.nverts = pFaceSizes[i];
		tmp_ptr = realloc(tmp_ptr, sizeof(int) * face.nverts);

		assert(tmp_ptr != NULL);

		for(j = 0; j < face.nverts; ++j)
		{
			tmp_ptr[j] = pFaceVertexIndices[base + j];
		}
		base += face.nverts;

		face.verts = tmp_ptr;
		ply_put_element(ply, (void*)&face);
	}

	if(tmp_ptr != NULL)
	{
		free(tmp_ptr);
		tmp_ptr = NULL;
	}
	/* close the PLY file */
	ply_close(ply);
}

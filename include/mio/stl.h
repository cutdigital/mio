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

#ifndef __MIO_STL_H__
#define __MIO_STL_H__  1

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*
    Funcion to read in a [.stl|.stl-ascii] file that stores a single 3D mesh object (in ASCII
    format). The pointer parameters will be allocated inside this function and must
    be freed by caller.
*/
void mioReadSTL(
	// absolute path to file
	const char* fpath,
	// pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
	double** pVertices,
	// pointer to list of offsets into "pVertices" stored as list of integers [i,j,k,...]
	// each entry is an offset into pVertices, where the vertices of a particular mesh/solid start
	double** pVertexOffsets,
	// pointer to list of normal coordinates associated with each face stored as [xyz,xyz,xyz,...]
	// NOTE: the number of normals is V * 3, where V is the number of vertices in "pVertices"
	double** pNormals,
	// number of vertices (which can be used to deduce the number of vertices)
	unsigned int* numVertices,
	// number of offset corresponding to number of solids in file
	unsigned int* numOffsets);

/*
    Funcion to write out a [.stl|.stl-ascii] file that stores a single 3D mesh object (in ASCII
    format).
*/
void mioWriteSTL(
    // absolute path to file
	const char* const fpath,
    // pointer to list of vertex coordinates stored as [xyz,xyz,xyz,...]
	const double* const pVertices,
    // pointer to list of offsets into "pVertices" stored as list of integers [i,j,k,...]
    // each entry is an offset into pVertices, where the vertices of a particular mesh/solid start
	const double* const pVertexOffsets,
    // pointer to list of normal coordinates associated with each face stored as [xyz,xyz,xyz,...]
    // NOTE: the number of normals is V * 3, where V is the number of vertices in "pVertices"
	const double* const pNormals,
    // number of triangles (which can be used to deduce the number of vertices) 
    const unsigned int numVertices,
	// number of offset corresponding to number of solids in file
	const unsigned int numOffsets);

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef __MIO_STL_H__
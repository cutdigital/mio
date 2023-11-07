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

#include "mio/mio.h"

#if defined(_WIN32)

ssize_t getline(char** lineptr, size_t* n, FILE* stream)
{
	size_t pos;
	int c;

	if(lineptr == NULL || stream == NULL || n == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	c = getc(stream);
	if(c == EOF)
	{
		return -1;
	}

	if(*lineptr == NULL)
	{
		*lineptr = (char*)malloc(128);
		if(*lineptr == NULL)
		{
			return -1;
		}
		*n = 128;
	}

	pos = 0;
	while(c != EOF)
	{
		if(pos + 1 >= *n)
		{
			size_t new_size = *n + (*n >> 2);
			if(new_size < 128)
			{
				new_size = 128;
			}
			char* new_ptr = (char*)realloc(*lineptr, new_size);
			if(new_ptr == NULL)
			{
				return -1;
			}
			*n = new_size;
			*lineptr = new_ptr;
		}

		((unsigned char*)(*lineptr))[pos++] = (unsigned char)c;
		if(c == '\n')
		{
			break;
		}
		c = getc(stream);
	}

	(*lineptr)[pos] = '\0';
	return pos;
}

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
)
{

}

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
)
{
    
}

#endif // #if defined (_WIN32)
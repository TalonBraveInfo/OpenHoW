/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <PL/platform_filesystem.h>

#include "extractor.h"
#include "fac.h"

/************************************************************/
/* Fac Triangle/Quad Faces Format */

typedef struct FacQuad {
	int8_t uv_coords[8];
	uint16_t vertex_indices[4];
	uint16_t normal_indices[4];
	uint32_t texture_index;
	uint16_t unknown[4];
} FacQuad;

static FacQuad *Fac_LoadQuads( PLFile *filePtr, unsigned int numQuads ) {
	bool status = false;
	FacQuad *quads = ( FacQuad * ) calloc( numQuads, sizeof( FacQuad ) );
	if ( quads == NULL ) {
		Warning( "Failed to allocate quads array!\n" );
		return NULL;
	}

	for ( unsigned int i = 0; i < numQuads; ++i ) {
		for ( unsigned int j = 0; j < 8; ++j ) {
			quads[ i ].uv_coords[ j ] = plReadInt8( filePtr, &status );
		}
		for ( unsigned int j = 0; j < 4; ++j ) {
			quads[ i ].vertex_indices[ j ] = plReadInt16( filePtr, false, &status );
		}
		for ( unsigned int j = 0; j < 4; ++j ) {
			quads[ i ].normal_indices[ j ] = plReadInt16( filePtr, false, &status );
		}
		quads[ i ].texture_index = plReadInt32( filePtr, false, &status );
		for ( unsigned int j = 0; j < 4; ++j ) {
			quads[ i ].unknown[ j ] = plReadInt16( filePtr, false, &status );
		}
	}

	if ( !status ) {
		free( quads );
		return NULL;
	}

	return quads;
}

static FacTriangle *Fac_LoadTriangles( PLFile *filePtr, unsigned int numTriangles ) {
	bool status = false;
	FacTriangle *triangles = ( FacTriangle * ) calloc( numTriangles, sizeof( FacTriangle ) );
	if ( triangles == NULL ) {
		Warning( "Failed to allocate triangle array!\n" );
		return NULL;
	}

	for ( unsigned int i = 0; i < numTriangles; ++i ) {
		for ( unsigned int j = 0; j < 6; ++j ) {
			triangles[ i ].uv_coords[ j ] = plReadInt8( filePtr, &status );
		}
		for ( unsigned int j = 0; j < 3; ++j ) {
			triangles[ i ].vertex_indices[ j ] = plReadInt16( filePtr, false, &status );
		}
		for ( unsigned int j = 0; j < 3; ++j ) {
			triangles[ i ].normal_indices[ j ] = plReadInt16( filePtr, false, &status );
		}
		triangles[ i ].unknown0 = plReadInt16( filePtr, false, &status );
		triangles[ i ].texture_index = plReadInt32( filePtr, false, &status );
		for ( unsigned int j = 0; j < 4; ++j ) {
			triangles[ i ].unknown1[ j ] = plReadInt16( filePtr, false, &status );
		}
	}

	if ( !status ) {
		free( triangles );
		return NULL;
	}

	return triangles;
}

FacHandle *Fac_LoadFile( const char *path ) {
	PLFile *filePtr = plOpenFile( path, false );
	if ( filePtr == NULL ) {
		Warning( "Failed to load Fac \"%s\", aborting!\nPL: %s\n", path, plGetError() );
		return NULL;
	}

	//LogDebug( "Opened Fac \"%s\"...\n", plGetFilePath( filePtr ) );

	/* 16 bytes of unknown data, just skip it for now */
	if ( !plFileSeek( filePtr, 16, PL_SEEK_CUR ) ) {
		plCloseFile( filePtr );

		Warning( "Failed to find data in Fac \"%s\"!\nPL: %s\n", path, plGetError() );
		return NULL;
	}

	bool status;
	uint32_t numTriangles = plReadInt32( filePtr, false, &status );
	FacTriangle *triangles = Fac_LoadTriangles( filePtr, numTriangles );
	if ( triangles == NULL && numTriangles > 0 ) {
		Warning( "Failed to read in triangles, \"%s\"!\nPL: %s\n", path, plGetError() );
		plCloseFile( filePtr );
		return NULL;
	}

	uint32_t numQuads = plReadInt32( filePtr, false, &status );
	FacQuad *quads = Fac_LoadQuads( filePtr, numQuads );
	if ( quads == NULL && numQuads > 0 ) {
		Warning( "Failed to read in quads, \"%s\"!\nPL: %s\n", path, plGetError() );
		free( triangles );
		plCloseFile( filePtr );
		return NULL;
	}

	if ( numQuads == 0 && numTriangles == 0 ) {
		Warning( "Fac \"%s\" contains no quads or triangles!\n", path );
	}

	// check for textures table
	uint8_t num_textures = plReadInt8( filePtr, &status );
	FacTextureIndex *texture_table = NULL;
	if ( num_textures > 0 ) {
		texture_table = ( FacTextureIndex * ) calloc( num_textures, sizeof( FacTextureIndex ) );
		if ( texture_table == NULL ) {
			Error( "Failed to allocate texture table for \"%s\"!\n", path );
		}
		for ( unsigned int i = 0; i < num_textures; ++i ) {
			plReadFile( filePtr, texture_table[ i ].name, 1, sizeof( texture_table[ i ].name ) );
		}
	}

	plCloseFile( filePtr );

	FacHandle *handle = ( FacHandle * ) calloc( 1, sizeof( FacHandle ) );
	if ( handle == NULL ) {
		Error( "Failed to allocate Fac handle for \"%s\"!\n", path );
	}
	handle->num_triangles = numTriangles + ( numQuads * 2 );
	handle->triangles = ( FacTriangle * ) calloc( handle->num_triangles, sizeof( FacTriangle ) );
	if ( handle->triangles == NULL ) {
		Error( "Failed to allocate triangle array for \"%s\"!\n", path );
	}

	for ( unsigned int i = 0; i < numTriangles; ++i ) {
		handle->triangles[ i ].texture_index = triangles[ i ].texture_index;
		for ( unsigned int j = 0, u = 0; j < 3; j++, u += 2 ) {
			handle->triangles[ i ].vertex_indices[ j ] = triangles[ i ].vertex_indices[ j ];
			handle->triangles[ i ].normal_indices[ j ] = triangles[ i ].normal_indices[ j ];
			handle->triangles[ i ].uv_coords[ u ] = triangles[ i ].uv_coords[ u ];
			handle->triangles[ i ].uv_coords[ u + 1 ] = triangles[ i ].uv_coords[ u + 1 ];
		}
	}

	for ( unsigned int i = 0, tri = numTriangles; i < numQuads; ++i ) {
		int quad_to_tri[2][3] = {
				{ 0, 1, 2 },
				{ 2, 3, 0 },
		};

		for ( int q = 0; q < 2; ++q, ++tri ) {
			int *q2t = quad_to_tri[ q ];

			handle->triangles[ tri ].texture_index = quads[ i ].texture_index;
			for ( unsigned int j = 0; j < 3; j++ ) {
				handle->triangles[ tri ].vertex_indices[ j ] = quads[ i ].vertex_indices[ q2t[ j ] ];
				handle->triangles[ tri ].normal_indices[ j ] = quads[ i ].normal_indices[ q2t[ j ] ];
				// todo
				handle->triangles[ tri ].uv_coords[ j * 2 ] = quads[ i ].uv_coords[ q2t[ j ] * 2 ];
				handle->triangles[ tri ].uv_coords[ j * 2 + 1 ] = quads[ i ].uv_coords[ q2t[ j ] * 2 + 1 ];
			}
		}
	}

	if ( texture_table != NULL ) {
		handle->texture_table = texture_table;
		handle->texture_table_size = num_textures;
	}

	return handle;
}

void Fac_WriteFile( FacHandle *handle, const char *path ) {
	FILE *fp = fopen( path, "wb" );
	if ( fp == NULL ) {
		Warning( "Failed to open, \"%s\"!\n", path );
		return;
	}

	// 16 bytes of unknown data, just skip it for now
	char empty[16];
	memset( empty, 0, sizeof( empty ) );
	fwrite( empty, sizeof( empty ), 1, fp );

	// write out the triangle data
	fwrite( &handle->num_triangles, sizeof( uint32_t ), 1, fp );
	struct __attribute__((packed)) {
		int8_t uv_coords[6];
		uint16_t vertex_indices[3];
		uint16_t normal_indices[3];
		uint16_t unknown0;
		uint32_t texture_index;
		uint16_t unknown1[4];
	} triangles[handle->num_triangles];
	memset( triangles, 0, sizeof( *triangles ) * handle->num_triangles );
	for ( unsigned int i = 0; i < handle->num_triangles; ++i ) {
		triangles[ i ].texture_index = handle->triangles[ i ].texture_index;
		for ( unsigned int j = 0; j < 3; ++j ) {
			triangles[ i ].vertex_indices[ j ] = handle->triangles[ i ].vertex_indices[ j ];
			triangles[ i ].normal_indices[ j ] = handle->triangles[ i ].normal_indices[ j ];
		}
		for ( unsigned int j = 0; j < 6; ++j ) {
			triangles[ i ].uv_coords[ j ] = handle->triangles[ i ].uv_coords[ j ];
		}
	}
	fwrite( triangles, sizeof( *triangles ), handle->num_triangles, fp );

	// we won't write any quads, so just mark it as 0
	uint32_t quads = 0;
	fwrite( &quads, sizeof( uint32_t ), 1, fp );

	// now write the string table
	if ( handle->texture_table_size > 0 ) {
		fwrite( &handle->texture_table_size, sizeof( uint8_t ), 1, fp );
		for ( unsigned int i = 0; i < handle->texture_table_size; ++i ) {
			fwrite( handle->texture_table[ i ].name, 1, 16, fp );
		}
	}

	// Done!
	fclose( fp );
}

void Fac_DestroyHandle( FacHandle *handle ) {
	if ( handle == NULL ) {
		return;
	}

	free( handle->triangles );
	free( handle->texture_table );
	free( handle );
}

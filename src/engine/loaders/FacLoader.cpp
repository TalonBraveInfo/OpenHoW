// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Utilities.h"
#include "FacLoader.h"

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
	FacQuad *quads = ( FacQuad * ) u_alloc( numQuads, sizeof( FacQuad ), true );
	for ( unsigned int i = 0; i < numQuads; ++i ) {
		for ( unsigned int j = 0; j < 8; ++j ) {
			quads[ i ].uv_coords[ j ] = PlReadInt8( filePtr, &status );
		}
		for ( unsigned int j = 0; j < 4; ++j ) {
			quads[ i ].vertex_indices[ j ] = PlReadInt16( filePtr, false, &status );
		}
		for ( unsigned int j = 0; j < 4; ++j ) {
			quads[ i ].normal_indices[ j ] = PlReadInt16( filePtr, false, &status );
		}
		quads[ i ].texture_index = PlReadInt32( filePtr, false, &status );
		for ( unsigned int j = 0; j < 4; ++j ) {
			quads[ i ].unknown[ j ] = PlReadInt16( filePtr, false, &status );
		}
	}

	if ( !status ) {
		PlFree( quads );
	}

	return quads;
}

static FacTriangle *Fac_LoadTriangles( PLFile *filePtr, unsigned int numTriangles ) {
	bool status = false;
	FacTriangle *triangles = ( FacTriangle * ) u_alloc( numTriangles, sizeof( FacTriangle ), true );
	for ( unsigned int i = 0; i < numTriangles; ++i ) {
		for ( unsigned int j = 0; j < 6; ++j ) {
			triangles[ i ].uv_coords[ j ] = PlReadInt8( filePtr, &status );
		}
		for ( unsigned int j = 0; j < 3; ++j ) {
			triangles[ i ].vertex_indices[ j ] = PlReadInt16( filePtr, false, &status );
		}
		for ( unsigned int j = 0; j < 3; ++j ) {
			triangles[ i ].normal_indices[ j ] = PlReadInt16( filePtr, false, &status );
		}
		triangles[ i ].unknown0 = PlReadInt16( filePtr, false, &status );
		triangles[ i ].texture_index = PlReadInt32( filePtr, false, &status );
		for ( unsigned int j = 0; j < 4; ++j ) {
			triangles[ i ].unknown1[ j ] = PlReadInt16( filePtr, false, &status );
		}
	}

	if ( !status ) {
		PlFree( triangles );
	}

	return triangles;
}

FacHandle *Fac_LoadFile( const char *path ) {
	PLFile *filePtr = PlOpenFile( path, false );
	if ( filePtr == NULL ) {
		Warning( "Failed to load Fac \"%s\", aborting!\nPL: %s\n", path, PlGetError() );
		return NULL;
	}

	//LogDebug( "Opened Fac \"%s\"...\n", plGetFilePath( filePtr ) );

	/* 16 bytes of unknown data, just skip it for now */
	if ( !PlFileSeek( filePtr, 16, PL_SEEK_CUR ) ) {
		PlCloseFile( filePtr );

		Warning( "Failed to find data in Fac \"%s\"!\nPL: %s\n", path, PlGetError() );
		return NULL;
	}

	bool status;
	uint32_t numTriangles = PlReadInt32( filePtr, false, &status );
	FacTriangle *triangles = Fac_LoadTriangles( filePtr, numTriangles );
	if ( triangles == NULL && numTriangles > 0 ) {
		Warning( "Failed to read in triangles, \"%s\"!\nPL: %s\n", path, PlGetError() );
		PlCloseFile( filePtr );
		return NULL;
	}

	uint32_t numQuads = PlReadInt32( filePtr, false, &status );
	FacQuad *quads = Fac_LoadQuads( filePtr, numQuads );
	if ( quads == NULL && numQuads > 0 ) {
		Warning( "Failed to read in quads, \"%s\"!\nPL: %s\n", path, PlGetError() );
		PlFree( triangles );
		PlCloseFile( filePtr );
		return NULL;
	}

	if ( numQuads == 0 && numTriangles == 0 ) {
		Warning( "Fac \"%s\" contains no quads or triangles!\n", path );
	}

	// check for textures table
	uint8_t num_textures = PlReadInt8( filePtr, &status );
	FacTextureIndex *texture_table = NULL;
	if ( num_textures > 0 ) {
		texture_table = ( FacTextureIndex * ) u_alloc( num_textures, sizeof( FacTextureIndex ), true );
		for ( unsigned int i = 0; i < num_textures; ++i ) {
			PlReadFile( filePtr, texture_table[ i ].name, 1, sizeof( texture_table[ i ].name ) );
		}
	}

	PlCloseFile( filePtr );

	FacHandle *handle = ( FacHandle * ) u_alloc( 1, sizeof( FacHandle ), true );
	handle->num_triangles = numTriangles + ( numQuads * 2 );
	handle->triangles = ( FacTriangle * ) u_alloc( handle->num_triangles, sizeof( FacTriangle ), true );

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
	u_fclose( fp );
}

void Fac_DestroyHandle( FacHandle *handle ) {
	if ( handle == NULL ) {
		return;
	}

	PlFree( handle->triangles );
	PlFree( handle->texture_table );
	PlFree( handle );
}

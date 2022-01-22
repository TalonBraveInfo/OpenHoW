// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "loaders/Loaders.h"

/************************************************************/
/* PSX Min Model Format */

MinHandle *Min_LoadFile( const char *path ) {
	PLFile *fp = PlOpenFile( path, false );
	if ( fp == NULL ) {
		Warning( "Failed to load Min \"%s\", aborting!\n", path );
		return NULL;
	}

	PlFileSeek( fp, 16, PL_SEEK_CUR );

	uint32_t num_triangles;
	if ( PlReadFile( fp, &num_triangles, sizeof( uint32_t ), 1 ) != 1 ) {
		Warning( "Failed to get number of triangles, \"%s\"!\n", path );
	}

	struct __attribute__((packed)) {
#if 0
		int8_t      uv_coords[6];
		uint16_t    vertex_indices[3];
		uint16_t    normal_indices[3];
		uint16_t    unknown0;
		uint32_t    texture_index;
		uint16_t    unknown1[4];
#else
		char u0[24];
#endif
	} triangles[num_triangles];
	static_assert( sizeof( *triangles ) == 24, "invalid struct size" );
	if ( PlReadFile( fp, triangles, sizeof( *triangles ), num_triangles ) != num_triangles ) {
		PlCloseFile( fp );
		Warning( "Failed to get %u triangles, \"%s\", aborting!\n", num_triangles, path );
		return NULL;
	}
	// todo
	return NULL;
}

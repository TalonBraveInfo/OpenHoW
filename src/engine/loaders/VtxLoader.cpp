// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Utilities.h"
#include "VtxLoader.h"

/************************************************************/
/* Vtx Vertex Format */

VtxHandle *Vtx_LoadFile( const char *path ) {
	PLFile *vtx_file = PlOpenFile( path, false );
	if ( vtx_file == NULL ) {
		Warning( "Failed to load Vtx \"%s\", aborting!\n", path );
		return NULL;
	}

	/* load in the vertices */

	typedef struct __attribute__((packed)) VtxCoord {
		int16_t v[3];
		uint16_t bone_index;
	} VtxCoord;
	unsigned int num_vertices = ( unsigned int ) ( PlGetFileSize( vtx_file ) / sizeof( VtxCoord ) );
	if ( num_vertices >= VTX_MAX_VERTICES ) {
		PlCloseFile( vtx_file );
		Warning( "Invalid number of vertices in \"%s\" (%d/%d)!\n", path, num_vertices, VTX_MAX_VERTICES );
		return NULL;
	}

	VtxCoord vertices[num_vertices];
	unsigned int rnum_vertices = PlReadFile( vtx_file, vertices, sizeof( VtxCoord ), num_vertices );
	PlCloseFile( vtx_file );

	if ( num_vertices == 0 ) {
		Warning( "No vertices found in Vtx \"%s\"!\n", path );
		return NULL;
	}

	if ( rnum_vertices != num_vertices ) {
		Warning( "Failed to read in all vertices from \"%s\", aborting!\n", path );
		return NULL;
	}

	VtxHandle *handle = ( VtxHandle * ) ohw::GetApp()->CAlloc( 1, sizeof( VtxHandle ), true );
	handle->vertices = ( PLGVertex * ) ohw::GetApp()->CAlloc( num_vertices, sizeof( PLGVertex ), true );
	handle->num_vertices = num_vertices;
	for ( unsigned int i = 0; i < num_vertices; ++i ) {
		handle->vertices[ i ].position = hei::Vector3( vertices[ i ].v[ 0 ], vertices[ i ].v[ 1 ], vertices[ i ].v[ 2 ] );
		//handle->vertices[ i ].bone_index = vertices[ i ].bone_index;
		handle->vertices[ i ].colour = PL_COLOUR_WHITE;
	}
	return handle;
}

void Vtx_DestroyHandle( VtxHandle *handle ) {
	if ( handle == NULL ) {
		return;
	}

	PlFree( handle->vertices );
	PlFree( handle );
}

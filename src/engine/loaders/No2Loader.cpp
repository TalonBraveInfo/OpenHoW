// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Utilities.h"
#include "No2Loader.h"

/************************************************************/
/* No2 Normals Format */

/**
 * @brief Loads in vertex normal data
 * @param path Path to the NO2 file
 * @param vertex_data Pointer to an existing VtxHandle which will be filled with normal data
 * @return Returns vertex_data on success, null on fail
 */
No2Handle *No2_LoadFile( const char *path ) {
	PLFile *fp = PlOpenFile( path, false );
	if ( fp == NULL ) {
		Warning( "Failed to load no2 \"%s\"!\n", path );
		return NULL;
	}

	typedef struct __attribute__((packed)) No2Coord {
		float v[3];
		float bone_index;
	} No2Coord;

	unsigned int numNormals = ( unsigned int ) ( PlGetFileSize( fp ) / sizeof( No2Coord ) );
	No2Coord *normals = ( No2Coord * ) ohw::GetApp()->CAlloc( numNormals, sizeof( No2Coord ), true );
	unsigned int numReadNormals = PlReadFile( fp, normals, sizeof( No2Coord ), numNormals );

	PlCloseFile( fp );

	if ( numReadNormals != numNormals ) {
		free( normals );
		Warning( "Failed to read in all normals from \"%s\"!\n", path );
		return NULL;
	}

	No2Handle *handle = ( No2Handle * ) ohw::GetApp()->MAlloc( sizeof( No2Handle ), true );
	handle->numNormals = numNormals;
	handle->normals = ( PLVector3 * ) ohw::GetApp()->MAlloc( sizeof( PLVector3 ) * handle->numNormals, true );
	for ( unsigned int i = 0; i < numNormals; ++i ) {
		handle->normals[ i ].x = normals->v[ 0 ];
		handle->normals[ i ].y = normals->v[ 1 ];
		handle->normals[ i ].z = normals->v[ 2 ];
	}

	PlFree( normals );

	return handle;
}

void No2_DestroyHandle( No2Handle *handle ) {
	PlFree( handle->normals );
	PlFree( handle );
}

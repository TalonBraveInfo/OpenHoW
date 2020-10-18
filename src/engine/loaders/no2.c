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
#include <PL/platform_mesh.h>

#include "Utilities.h"
#include "no2.h"

/************************************************************/
/* No2 Normals Format */

/**
 * @brief Loads in vertex normal data
 * @param path Path to the NO2 file
 * @param vertex_data Pointer to an existing VtxHandle which will be filled with normal data
 * @return Returns vertex_data on success, null on fail
 */
No2Handle *No2_LoadFile( const char *path ) {
	PLFile *fp = plOpenFile( path, false );
	if ( fp == NULL) {
		Warning( "Failed to load no2 \"%s\"!\n", path );
		return NULL;
	}

	typedef struct __attribute__((packed)) No2Coord {
		float v[3];
		float bone_index;
	} No2Coord;

	unsigned int numNormals = ( unsigned int ) ( plGetFileSize( fp ) / sizeof( No2Coord ));
	No2Coord *normals = malloc( sizeof( No2Coord ) * numNormals );
	unsigned int numReadNormals = plReadFile( fp, normals, sizeof( No2Coord ), numNormals );

	plCloseFile( fp );

	if ( numReadNormals != numNormals ) {
		free( normals );
		Warning( "Failed to read in all normals from \"%s\"!\n", path );
		return NULL;
	}

	No2Handle *handle = malloc( sizeof( No2Handle ) );
	handle->numNormals = numNormals;
	handle->normals = malloc( sizeof( PLVector3 ) * handle->numNormals );
	for ( unsigned int i = 0; i < numNormals; ++i ) {
		handle->normals[ i ].x = normals->v[ 0 ];
		handle->normals[ i ].y = normals->v[ 1 ];
		handle->normals[ i ].z = normals->v[ 2 ];
	}

	free( normals );

	return handle;
}

void No2_DestroyHandle( No2Handle *handle ) {
	free( handle->normals );
	free( handle );
}

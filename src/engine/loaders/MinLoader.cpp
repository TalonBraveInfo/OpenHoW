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

#include "App.h"
#include "loaders/Loaders.h"

/************************************************************/
/* PSX Min Model Format */

MinHandle *Min_LoadFile( const char *path ) {
	PLFile *fp = plOpenFile( path, false );
	if ( fp == NULL ) {
		Warning( "Failed to load Min \"%s\", aborting!\n", path );
		return NULL;
	}

	plFileSeek( fp, 16, PL_SEEK_CUR );

	uint32_t num_triangles;
	if ( plReadFile( fp, &num_triangles, sizeof( uint32_t ), 1 ) != 1 ) {
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
	if ( plReadFile( fp, triangles, sizeof( *triangles ), num_triangles ) != num_triangles ) {
		plCloseFile( fp );
		Warning( "Failed to get %u triangles, \"%s\", aborting!\n", num_triangles, path );
		return NULL;
	}
	// todo
	return NULL;
}

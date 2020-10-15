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

#include "util.h"
#include "vtx.h"

/************************************************************/
/* Vtx Vertex Format */

VtxHandle* Vtx_LoadFile(const char* path) {
  PLFile* vtx_file = plOpenFile(path, false);
  if (vtx_file == NULL) {
    Warning( "Failed to load Vtx \"%s\", aborting!\n", path);
    return NULL;
  }

  /* load in the vertices */

	typedef struct __attribute__((packed)) VtxCoord {
		int16_t v[3];
		uint16_t bone_index;
	} VtxCoord;
	unsigned int num_vertices = ( unsigned int ) ( plGetFileSize( vtx_file ) / sizeof( VtxCoord ) );
  if (num_vertices >= VTX_MAX_VERTICES) {
    plCloseFile(vtx_file);
    Warning( "Invalid number of vertices in \"%s\" (%d/%d)!\n", path, num_vertices, VTX_MAX_VERTICES);
    return NULL;
  }

  VtxCoord vertices[num_vertices];
  unsigned int rnum_vertices = plReadFile(vtx_file, vertices, sizeof(VtxCoord), num_vertices);
  plCloseFile(vtx_file);

  if (num_vertices == 0) {
    Warning( "No vertices found in Vtx \"%s\"!\n", path);
    return NULL;
  }

  if (rnum_vertices != num_vertices) {
    Warning( "Failed to read in all vertices from \"%s\", aborting!\n", path);
    return NULL;
  }

  VtxHandle* handle = u_alloc(1, sizeof(VtxHandle), true);
  handle->vertices = u_alloc(num_vertices, sizeof(PLVertex), true);
  handle->num_vertices = num_vertices;
  for (unsigned int i = 0; i < num_vertices; ++i) {
    handle->vertices[i].position = PLVector3(vertices[i].v[0], vertices[i].v[1], vertices[i].v[2]);
    handle->vertices[i].bone_index = vertices[i].bone_index;
    handle->vertices[i].colour = PL_COLOUR_WHITE;
  }
  return handle;
}

void Vtx_DestroyHandle(VtxHandle* handle) {
  if (handle == NULL) {
    return;
  }

  u_free(handle->vertices);
  u_free(handle);
}

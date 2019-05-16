/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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
#include "../engine.h"
#include "loaders.h"

/************************************************************/
/* Vtx Vertex Format */

VtxHandle* Vtx_LoadFile(const char* path) {
    FILE *vtx_file = fopen(path, "rb");
    if(vtx_file == NULL) {
        LogWarn("Failed to load Vtx \"%s\", aborting!\n", path);
        return NULL;
    }

    /* load in the vertices */

    typedef struct __attribute__((packed)) VtxCoord {
        int16_t     v[3];
        uint16_t    bone_index;
    } VtxCoord;
    unsigned int num_vertices = (unsigned int)(plGetFileSize(path) / sizeof(VtxCoord));
    VtxCoord vertices[num_vertices];
    unsigned int rnum_vertices = fread(vertices, sizeof(VtxCoord), num_vertices, vtx_file);
    u_fclose(vtx_file);

    if(num_vertices == 0) {
        LogWarn("No vertices found in Vtx \"%s\"!\n", path);
        return NULL;
    }

    if(rnum_vertices != num_vertices) {
        LogWarn("Failed to read in all vertices from \"%s\", aborting!\n", path);
        return NULL;
    }

    VtxHandle* handle = u_alloc(1, sizeof(VtxHandle), true);
    handle->vertices = u_alloc(num_vertices, sizeof(PLVertex), true);
    handle->num_vertices = num_vertices;
    for(unsigned int i = 0; i < num_vertices; ++i) {
        handle->vertices[i].position = PLVector3(vertices[i].v[0], vertices[i].v[1], vertices[i].v[2]);
        handle->vertices[i].bone_index = vertices[i].bone_index;
        handle->vertices[i].colour = PL_COLOUR_WHITE;
    }
    return handle;
}

void Vtx_DestroyHandle(VtxHandle* handle) {
    if(handle == NULL) {
        return;
    }

    u_free(handle->vertices);
    u_free(handle);
}

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
/* Fac Triangle/Quad Faces Format */

FacHandle* Fac_LoadFile(const char* path) {
    FILE* fac_file = fopen(path, "rb");
    if(fac_file == NULL) {
        LogWarn("Failed to load Fac \"%s\", aborting!\n", path);
        return NULL;
    }

    /* 16 bytes of unknown data, just skip it for now */
    fseek(fac_file, 16, SEEK_CUR);

    uint32_t num_triangles;
    if(fread(&num_triangles, sizeof(uint32_t), 1, fac_file) != 1) {
        LogWarn("Failed to get number of triangles, \"%s\"!\n", path);
    }

    if(num_triangles >= MAX_MODEL_TRIANGLES) {
        u_fclose(fac_file);
        LogWarn("Invalid number of triangles in \"%s\" (%d/%d)!\n", path, num_triangles, MAX_MODEL_TRIANGLES);
        return NULL;
    }

    struct __attribute__((packed)) {
        int8_t      uv_coords[6];
        uint16_t    vertex_indices[3];
        uint16_t    normal_indices[3];
        uint16_t    unknown0;
        uint32_t    texture_index;
        uint16_t    unknown1[4];
    } triangles[num_triangles];
    if(fread(triangles, sizeof(*triangles), num_triangles, fac_file) != num_triangles) {
        u_fclose(fac_file);
        LogWarn("Failed to get %u triangles, \"%s\", aborting!\n", num_triangles, path);
        return NULL;
    }

    uint32_t num_quads;
    if(fread(&num_quads, sizeof(uint32_t), 1, fac_file) != 1) {
        u_fclose(fac_file);
        LogWarn("Failed to get number of quads, \"%s\", aborting!\n", path);
        return NULL;
    }

    if(num_quads >= MAX_MODEL_TRIANGLES) {
        u_fclose(fac_file);
        LogWarn("Invalid number of quads in \"%s\" (%d/%d)!\n", path, num_triangles, MAX_MODEL_TRIANGLES);
        return NULL;
    }

    struct __attribute__((packed)) {
        int8_t      uv_coords[8];
        uint16_t    vertex_indices[4];
        uint16_t    normal_indices[4];
        uint32_t    texture_index;
        uint16_t    unknown[4];
    } quads[num_quads];
    if(fread(quads, sizeof(*quads), num_quads, fac_file) != num_quads) {
        u_fclose(fac_file);
        LogWarn("Failed to get %u quads, \"%s\", aborting!\n", num_quads, path);
        return NULL;
    }

    unsigned int total_triangles = num_triangles + (num_quads * 2);
    if(num_triangles >= MAX_MODEL_TRIANGLES) {
        u_fclose(fac_file);
        LogWarn("Invalid number of triangles in \"%s\" (%d/%d)!\n", path, num_triangles, MAX_MODEL_TRIANGLES);
        return NULL;
    }

    FacHandle* handle = u_alloc(1, sizeof(FacHandle), true);
    handle->num_triangles = total_triangles;
    handle->triangles = u_alloc(total_triangles, sizeof(FacTriangle), true);
    for(unsigned int i = 0; i < num_triangles; ++i) {
        handle->triangles[i].texture_index = triangles[i].texture_index;
        for(unsigned int j = 0, coord = 0; j < 3; j++) {
            handle->triangles[i].vertex_indices[j]  = triangles[i].vertex_indices[j];
            handle->triangles[i].normal_indices[j]  = triangles[i].normal_indices[j];
            handle->triangles[i].uv_coords[j][0]    = plByteToFloat(triangles[i].uv_coords[j + (coord++)]);
            handle->triangles[i].uv_coords[j][1]    = plByteToFloat(triangles[i].uv_coords[j + (coord++)]);
        }
    }
    for(unsigned int i = 0, tri = num_triangles; i < num_quads; ++i) {
        handle->triangles[tri].texture_index = quads[i].texture_index;
        for(unsigned int j = 0; j < 3; j++) {
            handle->triangles[tri].vertex_indices[j] = quads[i].vertex_indices[j];
            handle->triangles[tri].normal_indices[j] = quads[i].normal_indices[j];
            handle->triangles[tri].uv_coords[j][0] = 0;
            handle->triangles[tri].uv_coords[j][1] = 0;
        }
        tri++;
        uint8_t order[]={3, 0, 2};
        for(unsigned int j = 0; j < 3; j++) {
            handle->triangles[tri].vertex_indices[j] = quads[i].vertex_indices[order[j]];
            handle->triangles[tri].normal_indices[j] = quads[i].normal_indices[order[j]];
            handle->triangles[tri].uv_coords[j][0] = 0;
            handle->triangles[tri].uv_coords[j][1] = 0;
        }
        tri++;
    }

    return handle;
}

void Fac_DestroyHandle(FacHandle* handle) {
    if(handle == NULL) {
        return;
    }

    u_free(handle->triangles);
    u_free(handle);
}

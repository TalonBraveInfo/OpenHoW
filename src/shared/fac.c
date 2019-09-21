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
#include <PL/platform_mesh.h>

#include "util.h"
#include "fac.h"

/************************************************************/
/* Fac Triangle/Quad Faces Format */

FacHandle *Fac_LoadFile(const char *path) {
  FILE *fac_file = fopen(path, "rb");
  if (fac_file == NULL) {
    LogWarn("Failed to load Fac \"%s\", aborting!\n", path);
    return NULL;
  }

  /* 16 bytes of unknown data, just skip it for now */
  fseek(fac_file, 16, SEEK_CUR);

  uint32_t num_triangles;
  if (fread(&num_triangles, sizeof(uint32_t), 1, fac_file) != 1) {
    LogWarn("Failed to get number of triangles, \"%s\"!\n", path);
  }

  /* some models can have 0 triangles, as they'll use quads instead */
  if (num_triangles >= FAC_MAX_TRIANGLES) {
    u_fclose(fac_file);
    LogWarn("Invalid number of triangles in \"%s\" (%d/%d)!\n", path, num_triangles, FAC_MAX_TRIANGLES);
    return NULL;
  }

  struct __attribute__((packed)) {
    int8_t uv_coords[6];
    uint16_t vertex_indices[3];
    uint16_t normal_indices[3];
    uint16_t unknown0;
    uint32_t texture_index;
    uint16_t unknown1[4];
  } triangles[num_triangles];
  if (fread(triangles, sizeof(*triangles), num_triangles, fac_file) != num_triangles) {
    u_fclose(fac_file);
    LogWarn("Failed to get %u triangles, \"%s\", aborting!\n", num_triangles, path);
    return NULL;
  }

  uint32_t num_quads;
  if (fread(&num_quads, sizeof(uint32_t), 1, fac_file) != 1) {
    u_fclose(fac_file);
    LogWarn("Failed to get number of quads, \"%s\", aborting!\n", path);
    return NULL;
  }

  if (num_quads >= FAC_MAX_TRIANGLES) {
    u_fclose(fac_file);
    LogWarn("Invalid number of quads in \"%s\" (%d/%d)!\n", path, num_quads, FAC_MAX_TRIANGLES);
    return NULL;
  }

  struct __attribute__((packed)) {
    int8_t uv_coords[8];
    uint16_t vertex_indices[4];
    uint16_t normal_indices[4];
    uint32_t texture_index;
    uint16_t unknown[4];
  } quads[num_quads];
  if (fread(quads, sizeof(*quads), num_quads, fac_file) != num_quads) {
    u_fclose(fac_file);
    LogWarn("Failed to get %u quads, \"%s\", aborting!\n", num_quads, path);
    return NULL;
  }

  // check for textures table
  uint8_t num_textures;
  FacTextureIndex *texture_table = NULL;
  if(fread(&num_textures, sizeof(uint8_t), 1, fac_file) == 1 && num_textures > 0) {
    texture_table = u_alloc(num_textures, sizeof(FacTextureIndex), true);
    for(unsigned int i = 0; i < num_textures; ++i) {
      fread(texture_table[i].name, 1, sizeof(texture_table[i].name), fac_file);
    }
  }

  u_fclose(fac_file);

  unsigned int total_triangles = num_triangles + (num_quads * 2);
  if (total_triangles == 0 || total_triangles >= FAC_MAX_TRIANGLES) {
    LogWarn("Invalid number of triangles in \"%s\" (%d/%d)!\n", path, num_triangles, FAC_MAX_TRIANGLES);
    return NULL;
  }

  FacHandle *handle = u_alloc(1, sizeof(FacHandle), true);
  handle->num_triangles = total_triangles;
  handle->triangles = u_alloc(total_triangles, sizeof(FacTriangle), true);

  for (unsigned int i = 0; i < num_triangles; ++i) {
    handle->triangles[i].texture_index = triangles[i].texture_index;
    for (unsigned int j = 0, u = 0; j < 3; j++, u += 2) {
      handle->triangles[i].vertex_indices[j] = triangles[i].vertex_indices[j];
      handle->triangles[i].normal_indices[j] = triangles[i].normal_indices[j];
      // todo
      handle->triangles[i].uv_coords[u]     = triangles[i].uv_coords[u];
      handle->triangles[i].uv_coords[u + 1] = triangles[i].uv_coords[u + 1];
    }
  }

  for (unsigned int i = 0, tri = num_triangles; i < num_quads; ++i) {
    int quad_to_tri[2][3] = {
        { 0, 1, 2 },
        { 2, 3, 0 },

        // { 3, 0, 1 },
        // { 1, 2, 3 },

        // { 3, 1, 2 },
        // { 0, 0, 0 },

        //{ 2, 3, 0 },
        //{ 0, 1, 2 },

        //{ 1, 2, 0 },
        //{ 3, 0, 2 }

        // { 1, 3, 0 },
        // { 1, 2, 3 },
    };

    for(int q = 0; q < 2; ++q, ++tri) {
      int *q2t = quad_to_tri[q];

      handle->triangles[tri].texture_index = quads[i].texture_index;
      for (unsigned int j = 0; j < 3; j++) {
        handle->triangles[tri].vertex_indices[j] = quads[i].vertex_indices[q2t[j]];
        handle->triangles[tri].normal_indices[j] = quads[i].normal_indices[q2t[j]];
        // todo
        handle->triangles[tri].uv_coords[j * 2]     = quads[i].uv_coords[q2t[j] * 2];
        handle->triangles[tri].uv_coords[j * 2 + 1] = quads[i].uv_coords[q2t[j] * 2 + 1];
      }
    }
#if 0
    handle->triangles[tri].texture_index = quads[i].texture_index;
    for (unsigned int j = 0, u = 0; j < 3; j++, u += 2) {
      handle->triangles[tri].vertex_indices[j] = quads[i].vertex_indices[j];
      handle->triangles[tri].normal_indices[j] = quads[i].normal_indices[j];
      // todo
      handle->triangles[tri].uv_coords[u]     = quads[i].uv_coords[u];
      handle->triangles[tri].uv_coords[u + 1] = quads[i].uv_coords[u + 1];
    }
    tri++;
    uint8_t order[] = {2, 3, 0};
    handle->triangles[tri].texture_index = quads[i].texture_index;
    for (unsigned int j = 0; j < 3; j++) {
      handle->triangles[tri].vertex_indices[j] = quads[i].vertex_indices[order[j]];
      handle->triangles[tri].normal_indices[j] = quads[i].normal_indices[order[j]];
      // todo
      handle->triangles[tri].uv_coords[j * 2]     = quads[i].uv_coords[order[j] * 2];
      handle->triangles[tri].uv_coords[j * 2 + 1] = quads[i].uv_coords[order[j] * 2 + 1];
    }
    tri++;
#endif
  }

  if(texture_table != NULL) {
    handle->texture_table = texture_table;
    handle->texture_table_size = num_textures;
  }

  return handle;
}

void Fac_WriteFile(FacHandle *handle, const char *path) {
  FILE *fp = fopen(path, "wb");
  if(fp == NULL) {
    LogWarn("Failed to open, \"%s\"!\n");
    return;
  }

  // 16 bytes of unknown data, just skip it for now
  char empty[16];
  memset(empty, 0, sizeof(empty));
  fwrite(empty, sizeof(empty), 1, fp);

  // write out the triangle data
  fwrite(&handle->num_triangles, sizeof(uint32_t), 1, fp);
  struct __attribute__((packed)) {
    int8_t uv_coords[6];
    uint16_t vertex_indices[3];
    uint16_t normal_indices[3];
    uint16_t unknown0;
    uint32_t texture_index;
    uint16_t unknown1[4];
  } triangles[handle->num_triangles];
  memset(triangles, 0, sizeof(*triangles) * handle->num_triangles);
  for(unsigned int i = 0; i < handle->num_triangles; ++i) {
    triangles[i].texture_index = handle->triangles[i].texture_index;
    for(unsigned int j = 0 ; j < 3; ++j) {
      triangles[i].vertex_indices[j] = handle->triangles[i].vertex_indices[j];
      triangles[i].normal_indices[j] = handle->triangles[i].normal_indices[j];
    }
    for(unsigned int j = 0; j < 6; ++j) {
      triangles[i].uv_coords[j] = handle->triangles[i].uv_coords[j];
    }
  }
  fwrite(triangles, sizeof(*triangles), handle->num_triangles, fp);

  // we won't write any quads, so just mark it as 0
  uint32_t quads = 0;
  fwrite(&quads, sizeof(uint32_t), 1, fp);

  // now write the string table
  if(handle->texture_table_size > 0) {
    fwrite(&handle->texture_table_size, sizeof(uint8_t), 1, fp);
    for (unsigned int i = 0; i < handle->texture_table_size; ++i) {
      fwrite(handle->texture_table[i].name, 1, 16, fp);
    }
  }

  // Done!
  u_fclose(fp);
}

void Fac_DestroyHandle(FacHandle *handle) {
  if (handle == NULL) {
    return;
  }

  u_free(handle->triangles);
  u_free(handle->texture_table);
  u_free(handle);
}

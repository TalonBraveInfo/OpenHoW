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
/* PSX Min Model Format */

MinHandle *Min_LoadFile(const char *path) {
  FILE *fp = fopen(path, "rb");
  if (fp == NULL) {
    LogWarn("Failed to load Min \"%s\", aborting!\n", path);
    return NULL;
  }

  fseek(fp, 16, SEEK_CUR);

  uint32_t num_triangles;
  if (fread(&num_triangles, sizeof(uint32_t), 1, fp) != 1) {
    LogWarn("Failed to get number of triangles, \"%s\"!\n", path);
  }

  if (num_triangles == 0 || num_triangles >= MAX_MODEL_TRIANGLES) {
    u_fclose(fp);
    LogWarn("Invalid number of triangles in \"%s\" (%d/%d)!\n", path, num_triangles, MAX_MODEL_TRIANGLES);
    return NULL;
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
  static_assert(sizeof(*triangles) == 24, "invalid struct size");
  if (fread(triangles, sizeof(*triangles), num_triangles, fp) != num_triangles) {
    u_fclose(fp);
    LogWarn("Failed to get %u triangles, \"%s\", aborting!\n", num_triangles, path);
    return NULL;
  }
  // todo
  return NULL;
}

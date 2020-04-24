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

#pragma once

/* Machinor Model Format */

#define MMF_IDENTIFIER  "MMF"

typedef struct MmfHeader {
  char ident[3];
  uint8_t version;
  uint32_t num_chunks;
  uint64_t chunk_offsets[num_chunks];
} MmfHeader;

typedef struct MmfChunkHeader {
  char ident[3];
  uint32_t length;
} MmfChunkHeader;

/* Vertices Chunk */

#define MMF_VERTICES_IDENTIFIER "VTX"

typedef struct MmfVerticesVertex {

};

typedef struct MmfVerticesHeader {
  MmfChunkHeader chunk_info;
  uint32_t num_vertices;
  MmfVerticesVertex vertices[num_vertices];
};

/* Faces Chunk */

#define MMF_FACES_IDENTIFIER  "FAC"

typedef struct MmfFacesTriangle {
  float uv_coords[3][2];        // converted uv coords
  uint32_t vertex_indices[3];
  uint32_t normal_indices[3];
  uint8_t texture_index;        // index into textures table
} MmfFacesTriangle;

typedef struct MmfFacesTextureTableIndex {
  uint8_t string_length;
  char name[string_length];
} MmfFacesTextureTableIndex;

typedef struct MmfFacesHeader {
  MmfChunkHeader chunk_info;
  uint32_t num_triangles;
  MmfFacesTriangle triangles[num_triangles];
  uint32_t num_textures;
  MmfFacesTextureTableIndex textures[num_textures];
} MmfFacesHeader;

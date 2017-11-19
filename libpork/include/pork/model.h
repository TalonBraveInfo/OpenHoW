/* OpenHOW
 * Copyright (C) 2017 Mark E Sowden
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

#include <pork/pork.h>

//////////////////////////////////////////////////////////////////////////////////

#define MAX_MODELS  4096    // originally I was thinking to size this dynamically, but is there even a need?

#define MAX_VERTICES    2048
#define MAX_TRIANGLES   4096
#define MAX_QUADS       4096
#define MAX_BONES       32

typedef struct Model {

    FACTriangle triangles[MAX_TRIANGLES];
    FACQuad     quads[MAX_QUADS];

    unsigned int num_vertices;
    unsigned int num_triangles; // triangles * (quads * 2)
    unsigned int num_bones;
    unsigned int num_meshes;

    PLMesh *tri_mesh;       // Our actual output!
    PLMesh *skeleton_mesh;  // preview of skeleton

    PLMesh **meshes;
} Model;

void init_model_cache(void);
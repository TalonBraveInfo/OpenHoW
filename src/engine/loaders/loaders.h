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

#pragma once

/* Hogs of War file formats
 * -----------------------------------
 * BIN : Audio/Model/Texture data
 * DAT : Audio data
 * MAM : Mangled texture / model package
 * MAD : Texture / model package
 * MTM : Mangled model texture package
 * MTD : Texture / model package
 * MMM : Mangled model package
 * MGL : Mangled texture data
 * FAC : Model faces
 * VTX : Model vertices
 * NO2 : Model normals
 * HIR : Model skeleton
 * POM : Mangled map object data
 * POG : Map object data
 * PTM : Mangled map textures package
 * PTG : Map textures package
 * PMM : Mangled terrain data
 * PMG : Terrain data
 * OFF : File offset sizes
 */

#define MAX_MODEL_VERTICES  4096
#define MAX_MODEL_TRIANGLES 8192

PL_EXTERN_C

typedef struct VtxHandle {
    struct PLVertex*    vertices;
    unsigned int        num_vertices;
} VtxHandle;
VtxHandle* Vtx_LoadFile(const char* path);
void Vtx_DestroyHandle(VtxHandle* handle);

VtxHandle* No2_LoadFile(const char *path, VtxHandle* vertex_data);;

typedef struct HirHandle {
    PLModelBone*    bones;
    unsigned int    num_bones;
} HirHandle;
HirHandle* Hir_LoadFile(const char* path);
void Hir_DestroyHandle(HirHandle* handle);

void RegisterPackageLoaders(void);

PL_EXTERN_C_END

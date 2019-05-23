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
 * MAD : Texture / model package            (done)
 * MTM : Mangled model texture package
 * MTD : Texture / model package            (done)
 * MMM : Mangled model package
 * MGL : Mangled texture data
 * MIN : PSX model data
 * FAC : Model faces                        (done)
 * VTX : Model vertices                     (done)
 * NO2 : Model normals                      (done)
 * HIR : Model skeleton                     (done)
 * POM : Mangled map object data
 * POG : Map object data                    (done)
 * PTM : Mangled map textures package
 * PTG : Map textures package               (done)
 * PMM : Mangled terrain data
 * PMG : Terrain data                       (done)
 * OFF : File offset sizes                  (done)
 */

#define MAX_MODEL_VERTICES  4096
#define MAX_MODEL_TRIANGLES 8192

PL_EXTERN_C

typedef struct FacTriangle {
    float       uv_coords[3][2];
    uint16_t    vertex_indices[3];
    uint16_t    normal_indices[3];
    uint32_t    texture_index;
} FacTriangle;

typedef struct FacHandle {
    FacTriangle*    triangles;
    unsigned int    num_triangles;
} FacHandle;
FacHandle* Fac_LoadFile(const char* path);
void Fac_DestroyHandle(FacHandle* handle);

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

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

#include "FacLoader.h"
#include "VtxLoader.h"
#include "No2Loader.h"

typedef enum HirSkeletonBone {
	PELVIS = 0,
	SPINE,
	HEAD,

	UPPER_ARM_L,
	LOWER_ARM_L,
	HAND_L,

	UPPER_ARM_R,
	LOWER_ARM_R,
	HAND_R,

	UPPER_LEG_L,
	LOWER_LEG_L,
	FOOT_L,

	UPPER_LEG_R,
	LOWER_LEG_R,
	FOOT_R,

	MAX_BONES
} HirSkeletonBone;

typedef struct HirHandle {
	PLModelBone *bones;
	unsigned int num_bones;
} HirHandle;
HirHandle *Hir_LoadFile( const char *path );
void Hir_DestroyHandle( HirHandle *handle );

typedef struct MinHandle {
	unsigned int blah;
} MinHandle;
MinHandle *Min_LoadFile( const char *path );

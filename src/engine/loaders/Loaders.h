// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

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
	PLMModelBone *bones;
	unsigned int num_bones;
} HirHandle;
HirHandle *Hir_LoadFile( const char *path );
void Hir_DestroyHandle( HirHandle *handle );

typedef struct MinHandle {
	unsigned int blah;
} MinHandle;
MinHandle *Min_LoadFile( const char *path );

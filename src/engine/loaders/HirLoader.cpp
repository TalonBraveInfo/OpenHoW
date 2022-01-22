// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Loaders.h"

/************************************************************/
/* Hir Skeleton Format */

HirHandle *Hir_LoadFile( const char *path ) {
	PLFile *file = PlOpenFile( path, false );
	if ( file == nullptr ) {
		Warning( "Failed to load \"%s\", aborting!\n", path );
		return nullptr;
	}

	size_t hir_size = PlGetFileSize( file );
	if ( hir_size == 0 ) {
		PlCloseFile( file );
		Warning( "Unexpected Hir size in \"%s\", aborting (%s)!\n", path, PlGetError() );
		return nullptr;
	}

	typedef struct __attribute__((packed)) HirBone {
		int32_t parent;
		int16_t coords[3];
		int8_t unknown[10];
	} HirBone;

	auto num_bones = ( unsigned int ) ( hir_size / sizeof( HirBone ) );
	HirBone bones[num_bones];
	unsigned int rnum_bones = PlReadFile( file, bones, sizeof( HirBone ), num_bones );
	PlCloseFile( file );

	if ( rnum_bones != num_bones ) {
		Warning( "Failed to read in all bones, %d/%d, aborting!\n", rnum_bones, num_bones );
		return nullptr;
	}

	/* for debugging */
	static const char *bone_names[static_cast<int>(HirSkeletonBone::MAX_BONES)] = {
			"Pelvis",
			"Spine",
			"Head",
			"UpperArm.L", "LowerArm.L", "Hand.L",
			"UpperArm.R", "LowerArm.R", "Hand.R",
			"UpperLeg.L", "LowerLeg.L", "Foot.L",
			"UpperLeg.R", "LowerLeg.R", "Foot.R",
	};

	/* in the long term, we won't have this here, we'll probably extend the format
	 * to include the names of each bone (.skeleton format?) */
	if ( static_cast<HirSkeletonBone>(num_bones) >= HirSkeletonBone::MAX_BONES ) {
		Warning( "Invalid number of bones, %d/%d, aborting!\n", num_bones, HirSkeletonBone::MAX_BONES );
		return nullptr;
	}

	auto *handle = static_cast<HirHandle *>(u_alloc( 1, sizeof( HirHandle ), true ));
	handle->bones = static_cast<PLMModelBone *>(u_alloc( num_bones, sizeof( PLMModelBone ), true ));
	for ( unsigned int i = 0; i < num_bones; ++i ) {
		handle->bones[ i ].position = hei::Vector3( bones[ i ].coords[ 0 ], bones[ i ].coords[ 1 ],
		                                            bones[ i ].coords[ 2 ] );
		handle->bones[ i ].parent = bones[ i ].parent;
		strcpy( handle->bones[ i ].name, bone_names[ i ] );
	}
	return handle;
}

void Hir_DestroyHandle( HirHandle *handle ) {
	if ( handle == nullptr ) {
		return;
	}

	PlFree( handle->bones );
	PlFree( handle );
}

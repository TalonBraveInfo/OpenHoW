// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include <plcore/pl.h>

#include "model.h"

const char *Model_GetAnimationDescription( unsigned int i ) {
	static const char *animationNames[] = {
		"Run cycle (normal)",
		"Run cycle (wounded)",
		"Run cycle (more wounded)",
		"Walk cycle (backwards)",
		"Turning on Spot",
		"Swimming",
		"Swimming like Rick",
		"Getting into Vehicles",
		"Jumping - Start",
		"Jumping - Middle",
		"Jumping - End",
		"Scramble",
		"Getting out Handgun",
		"Getting out Rifle",
		"Getting out Machine gun",
		"Getting out Heavy weapon",
		"Getting out Punch",
		"Getting out Grenade",
		"Getting out Sword / Knife",
		"Using Grenade",
		"Using Punch",
		"Sword / Knife",
		"Bayonet",
		"Aiming Handgun",
		"Aiming Rifle",
		"Aiming Machine gun",
		"Aiming Heavy weapon",
		"Standing around cycle 1",
		"Standing around cycle 2",
		"Very Wounded",
		"Lord Flash-Heart Pose",
		"Looking around",
		"Looking gormless",
		"Cowering",
		"Brushoff 1",
		"Brushoff 2",
		"Brushoff 3",
		"Sneeze",
		"Flying through air/falling",
		"Bouncing on B-Hind",
		"Getting to feet",
		"Celebration #1",
		"Celebration #2",
		"Celebration #3",
		"Salute",
		"Look back",
		"Thinking",
		"Dying #1",
		"Dying #2",
		"Dying #3",
		"Drowning",
		"Idle Cold",
		"Idle Hot",
		"Lay Mine",
		"Heal",
		"Pick pocket",
		"Air strike",
		"Hari Kiri",
		"Parachuting",
	};

	if ( i >= PL_ARRAY_ELEMENTS( animationNames ) ) {
		return nullptr;
	}

	return animationNames[ i ];
}

/************************************************************/

#if 0
/* cache the pigs data into memory, since we
 * share it between all of them anyway :) */
void CacheModelData() {
  memset(&model_cache, 0, sizeof(model_cache));

  model_cache.pig_skeleton = Hir_LoadFile(u_find("chars/pig.hir"));
  if (model_cache.pig_skeleton == nullptr) {
	Error("Failed to load skeleton, aborting!\n")
  }

  // animations

  LogInfo("Caching mcap.mad\n");

  char mcap_path[PL_SYSTEM_MAX_PATH];
  strncpy(mcap_path, u_find("chars/mcap.mad"), sizeof(mcap_path));

  // check the number of bytes making up the mcap; we'll use this
  // to determine the length of animations later
  size_t mcap_bytes = plGetFileSize(mcap_path);
  if (mcap_bytes < 272) {
	Error("Unexpected \"mcap.mad\" size, %d, aborting!\n", mcap_bytes)
  }

  FILE *file = fopen(mcap_path, "rb");
  if (file == nullptr) {
	Error("Failed to load \"%s\", aborting!\n", mcap_path)
  }

  // run through each animation and copy it
  // into our global animation list
  for (unsigned int i = 0; i < MAX_ANIMATIONS; ++i) {
	// basically a MAD package, but without the
	// file name
	struct __attribute__((packed)) {
	  uint32_t offset;
	  uint32_t length;
	} index{};
	if (fread(&index, sizeof(index), 1, file) != 1) {
	  Error("Failed to read index, aborting!\n")
	}

	// position we'll return to for the next index
	long position = ftell(file);

	/* MCAP Format Specification
	 * Used to store our piggy animations. */
	typedef struct __attribute__((packed)) McapKeyframe {
	  int16_t unused;

	  struct __attribute__((packed)) {
		int8_t x;
		int8_t y;
		int8_t z;
	  } transforms[10];

	  struct __attribute__((packed)) {
		float x;
		float y;
		float z;
		float w;
	  } rotations[15];
	} McapKeyframe;

	auto num_keyframes = (unsigned int) (index.length / sizeof(McapKeyframe));
	if (num_keyframes == 0) {
	  Error("Odd keyframe at index, aborting!\n")
	}

	// copy everything into our global animations array

	model_cache.animations[i].frames = static_cast<Keyframe *>(u_alloc(num_keyframes, sizeof(Keyframe), true));

	// move to where the first keyframe is
	if (fseek(file, index.offset, SEEK_SET) != 0) {
	  Error("Failed to seek to offset %d in file, aborting!\n", index.offset)
	}

	for (unsigned int j = 0; j < num_keyframes; ++j) {
	  // read in the keyframe data
	  McapKeyframe frame;
	  if (fread(&frame, sizeof(McapKeyframe), 1, file) != 1) {
		Error("Failed to read animation keyframe, aborting!\n")
	  }

	  // copy transforms
	  for (unsigned int k = 0; k < 10; ++k) {
		model_cache.animations[i].frames[j].transforms[k].x = frame.transforms[k].x;
		model_cache.animations[i].frames[j].transforms[k].y = frame.transforms[k].y;
		model_cache.animations[i].frames[j].transforms[k].z = frame.transforms[k].z;
	  }

	  // copy rotations
	  for (unsigned int k = 0; k < 15; ++k) {
		model_cache.animations[i].frames[j].rotations[k].x = frame.rotations[k].x;
		model_cache.animations[i].frames[j].rotations[k].y = frame.rotations[k].y;
		model_cache.animations[i].frames[j].rotations[k].z = frame.rotations[k].z;
		model_cache.animations[i].frames[j].rotations[k].w = frame.rotations[k].w;
	  }
	}

	model_cache.animations[i].num_frames = num_keyframes;
	if (i < plArrayElements(animation_names)) {
	  model_cache.animations[i].name = animation_names[i];
	}

	// return us from whence we came
	if (fseek(file, position, SEEK_SET) != 0) {
	  Error("Failed to seek back to original position %d in file, aborting!\n", position)
	}
  }
  u_fclose(file)

#if 0 // debug
  for(unsigned int i = 0; i < ANI_END; ++i) {
	  for(unsigned int j = 0; j < g_model_cache.animations[i].num_frames; ++j) {
		  print_debug("anim(%s) frame(%d) transformation(x:%d y:%d z:%d) angles(%d %d %d %d)\n",
					  g_model_cache.animations[i].name, j,

					  // print out the first transform
					  (int)model_cache.animations[i].frames[j].transforms[0].x,
					  (int)model_cache.animations[i].frames[j].transforms[0].y,
					  (int)model_cache.animations[i].frames[j].transforms[0].z,

					  // print out first rotation
					  (int)model_cache.animations[i].frames[j].rotations[0].x,
					  (int)model_cache.animations[i].frames[j].rotations[0].y,
					  (int)model_cache.animations[i].frames[j].rotations[0].z,
					  (int)model_cache.animations[i].frames[j].rotations[0].w
		  );
	  }
  }
#endif
}
#endif

#if 0
void DEBUGDrawSkeleton() {
  if (!cv_debug_skeleton->b_value) {
	return;
  }

  static PLMesh* skeleton_mesh = nullptr;
  if (skeleton_mesh == nullptr) {
	skeleton_mesh = plCreateMesh(PL_MESH_LINES, PL_DRAW_DYNAMIC, 0, model_cache.pig_skeleton->num_bones * 2);
  }

#if 0
  glPushMatrix();

  static float rotation = 90;
  rotation += 0.5f;
  glRotatef(0, 1, 0, 0);
  glRotatef(rotation, 0, 1, 0);
  glRotatef(180.f, 0, 0, 1);
#endif

  plClearMesh(skeleton_mesh);

  static unsigned int frame = 0;
  static double delay = 20;
  if (g_state.sys_ticks > delay) {
	frame++;
	delay = g_state.sys_ticks + 20;
  }

  if (frame == model_cache.animations[0].num_frames) {
	frame = 0;
  }

  for (unsigned int i = 0, vert = 0; i < model_cache.pig_skeleton->num_bones; ++i, vert += 2) {
	PLModelBone* bone = &model_cache.pig_skeleton->bones[i];

	//start
	plSetMeshVertexPosition(skeleton_mesh, vert, bone->position);
	plSetMeshVertexColour(skeleton_mesh, vert, PLColour(255, 0, 0, 255));

	//end
	plSetMeshVertexPosition(skeleton_mesh, vert + 1, model_cache.pig_skeleton->bones[bone->parent].position);
	plSetMeshVertexColour(skeleton_mesh, vert + 1, PLColour(0, 255, 0, 255));
  }

  plSetNamedShaderUniformMatrix4(NULL, "pl_model", plMatrix4Identity(), false);
  plUploadMesh(skeleton_mesh);
  plDrawMesh(skeleton_mesh);

#if 0
  glPopMatrix();
#endif
}
#endif

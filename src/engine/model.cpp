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
#include <PL/platform_model.h>

#include "engine.h"
#include "model.h"
#include "loaders/loaders.h"

#include "graphics/display.h"
#include "graphics/shader.h"
#include "graphics/texture_atlas.h"
#include "graphics/mesh.h"

struct {
  HirHandle *pig_skeleton;

  Animation animations[MAX_ANIMATIONS];
  unsigned int num_animations;
} model_cache;

static PLModel *Model_LoadVtxFile(const char *path) {
  VtxHandle *vtx = Vtx_LoadFile(path);
  if (vtx == nullptr) {
    LogWarn("Failed to load Vtx, \"%s\"!\n", path);
    return nullptr;
  }

  /* now we load in all the faces */
  char fac_path[PL_SYSTEM_MAX_PATH];
  strncpy(fac_path, path, strlen(path) - 3);
  fac_path[strlen(path) - 3] = '\0';
  strcat(fac_path, "fac");
  FacHandle *fac = Fac_LoadFile(fac_path);
  if (fac == nullptr) {
    Vtx_DestroyHandle(vtx);
    LogWarn("Failed to load Fac, \"%s\"!\n", path);
    return nullptr;
  }

  const char *filename = plGetFileName(path);
  // skydome is a special case, since we don't care about textures...
  if (pl_strcasecmp(filename, "skydome.vtx") == 0 || pl_strcasecmp(filename, "skydomeu.vtx") == 0) {
    PLMesh *mesh = plCreateMesh(PL_MESH_TRIANGLES, PL_DRAW_STATIC, fac->num_triangles, vtx->num_vertices);
    if (mesh == nullptr) {
      Vtx_DestroyHandle(vtx);
      Fac_DestroyHandle(fac);
      LogWarn("Failed to create mesh (%s)!\n", plGetError());
      return nullptr;
    }

    for (unsigned int j = 0; j < vtx->num_vertices; ++j) {
      plSetMeshVertexPosition(mesh, j, vtx->vertices[j].position * -1 * .5f);
    }

    unsigned int cur_index = 0;
    for (unsigned int j = 0; j < fac->num_triangles; ++j) {
      plSetMeshTrianglePosition(mesh, &cur_index,
                                fac->triangles[j].vertex_indices[0],
                                fac->triangles[j].vertex_indices[1],
                                fac->triangles[j].vertex_indices[2]
      );
    }

    Vtx_DestroyHandle(vtx);
    Fac_DestroyHandle(fac);

    mesh->texture = Display_GetDefaultTexture();

    PLModel *model = plCreateBasicStaticModel(mesh);
    if (model == nullptr) {
      LogWarn("Failed to create model (%s)!\n", plGetError());
      return nullptr;
    }

    return model;
  }

  TextureAtlas atlas(128, 8);
  if (fac->texture_table_size > 0) {
    for (unsigned int i = 0; i < fac->texture_table_size; ++i) {
      if (fac->texture_table[i].name[0] == '\0') {
        LogWarn("Invalid texture name in table, skipping (%d)!\n", i);
        continue;
      }

      std::string str = path;
      size_t pos = str.find_last_of('/');
      std::string texture_path = str.erase(pos) + "/";
      atlas.AddImage(texture_path + fac->texture_table[i].name + ".png", true);
    }
    atlas.Finalize();
  }

  PLMesh *mesh = plCreateMesh(PL_MESH_TRIANGLES, PL_DRAW_DYNAMIC, fac->num_triangles, fac->num_triangles * 3);
  if (mesh == nullptr) {
    Vtx_DestroyHandle(vtx);
    Fac_DestroyHandle(fac);
    LogWarn("Failed to create mesh (%s)!\n", plGetError());
    return nullptr;
  }

  for (unsigned int j = 0; j < vtx->num_vertices; ++j) {
    vtx->vertices[j].position *= .5f;
  }

  // automatically returns default if failed
  mesh->texture = atlas.GetTexture();

  unsigned int cur_index = 0;
  for (unsigned int j = 0, next_vtx_i = 0; j < fac->num_triangles; ++j) {
    for (unsigned int tri_vtx_i = 0; tri_vtx_i < 3; ++tri_vtx_i, ++next_vtx_i) {
      unsigned int tri_vtx = fac->triangles[j].vertex_indices[tri_vtx_i];

      plSetMeshVertexColour(mesh, next_vtx_i, PL_COLOUR_WHITE);
      plSetMeshVertexPosition(mesh, next_vtx_i, vtx->vertices[tri_vtx].position);

#if 0 // todo: only necessary for pigs?
      mesh->vertices[next_vtx_i].bone_index = vtx->vertices[tri_vtx].bone_index;
      mesh->vertices[next_vtx_i].bone_weight = 1.f;
#endif
    }

    plSetMeshTrianglePosition(mesh, &cur_index,
                              next_vtx_i - 1,
                              next_vtx_i - 2,
                              next_vtx_i - 3

                              /*
                              next_vtx_i - 3,
                              next_vtx_i - 2,
                              next_vtx_i - 1
                               */
    );

    if (fac->texture_table != nullptr) {
      float tx_x, tx_y, tx_w, tx_h;
      atlas.GetTextureCoords(fac->texture_table[fac->triangles[j].texture_index].name, &tx_x, &tx_y, &tx_w, &tx_h);

      std::pair<unsigned int, unsigned int>
          texture_size = atlas.GetTextureSize(fac->texture_table[fac->triangles[j].texture_index].name);

      for (unsigned int k = 0, u = 0; k < 3; ++k, u += 2) {
        plSetMeshVertexST(mesh, next_vtx_i - (3 - k),
                          tx_x + (tx_w * (1.0f / (float) (texture_size.first))
                              * (float) (fac->triangles[j].uv_coords[u])),
                          tx_y + (tx_h * (1.0f / (float) (texture_size.second))
                              * (float) (fac->triangles[j].uv_coords[u + 1])));
      }
    }
  }

  std::list<PLMesh *> meshes(&mesh, &mesh + 1);
  Mesh_GenerateFragmentedMeshNormals(meshes);

#if 0
  auto *skeleton =
      static_cast<PLModelBone *>(u_alloc(model_cache.pig_skeleton->num_bones, sizeof(PLModelBone), true));
  memcpy(skeleton, model_cache.pig_skeleton->bones, sizeof(PLModelBone) * model_cache.pig_skeleton->num_bones);
  PLModel *model = plCreateBasicSkeletalModel(mesh, skeleton, model_cache.pig_skeleton->num_bones, BONE_INDEX_PELVIS);
#else
  PLModel *model = plCreateBasicStaticModel(mesh);
#endif
  if (model == nullptr) {
    LogWarn("Failed to create model (%s)!\n", plGetError());
    return nullptr;
  }

  plGenerateModelBounds(model);

  return model;
}

PLModel *Model_LoadMinFile(const char *path) {
  u_assert(0, "TODO");
  return nullptr;
}

/************************************************************/

/* cache the pigs data into memory, since we
 * share it between all of them anyway :) */
void CacheModelData() {
  memset(&model_cache, 0, sizeof(model_cache));

#if 0
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

  /* todo, split this up, as the psx version deals with these as separate files */
  static const char *animation_names[] = {
      "Run cycle (normal)", "Run cycle (wounded)", "Run cycle (more wounded)", "Walk cycle (backwards)",
      "Turning on Spot",
      "Swimming", "Swimming like Rick",
      "Getting into Vehicles",
      "Jumping - Start", "Jumping - Middle", "Jumping - End",
      "Scramble",
      "Getting out Handgun", "Getting out Rifle", "Getting out Machine gun", "Getting out Heavy weapon",
      "Getting out Punch", "Getting out Grenade", "Getting out Sword / Knife",
      "Using Grenade", "Using Punch",
      "Sword / Knife",
      "Bayonet",
      "Aiming Handgun", "Aiming Rifle", "Aiming Machine gun", "Aiming Heavy weapon",
      "Standing around cycle 1", "Standing around cycle 2",
      "Very Wounded",
      "Lord Flash-Heart Pose",
      "Looking around", "Looking gormless",
      "Cowering",
      "Brushoff 1", "Brushoff 2", "Brushoff 3",
      "Sneeze",
      "Flying through air/falling",
      "Bouncing on B-Hind",
      "Getting to feet",
      "Celebration #1", "Celebration #2", "Celebration #3",
      "Salute",
      "Look back",
      "Thinking",
      "Dying #1", "Dying #2", "Dying #3",
      "Drowning",
      "Idle Cold", "Idle Hot",
      "Lay Mine",
      "Heal",
      "Pick pocket",
      "Air strike",
      "Hari Kiri",
      "Parachuting",
  };

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
#endif
}

void DEBUGDrawSkeleton() {
  if (!cv_debug_skeleton->b_value) {
    return;
  }

  static PLMesh *skeleton_mesh = nullptr;
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
    PLModelBone *bone = &model_cache.pig_skeleton->bones[i];

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

ModelManager *ModelManager::instance_ = nullptr;

ModelManager::ModelManager() {
  PLModel *LoadObjModel(const char *path); // see loaders/obj.cpp
  plRegisterModelLoader("obj", LoadObjModel);
  plRegisterModelLoader("vtx", Model_LoadVtxFile);
  plRegisterModelLoader("min", Model_LoadMinFile);

  PLMesh *default_mesh = plCreateMesh(PL_MESH_LINES, PL_DRAW_DYNAMIC, 0, 6);
  plSetMeshVertexPosition(default_mesh, 0, PLVector3(0, 20, 0));
  plSetMeshVertexPosition(default_mesh, 1, PLVector3(0, -20, 0));
  plSetMeshVertexPosition(default_mesh, 2, PLVector3(20, 0, 0));
  plSetMeshVertexPosition(default_mesh, 3, PLVector3(-20, 0, 0));
  plSetMeshVertexPosition(default_mesh, 4, PLVector3(0, 0, 20));
  plSetMeshVertexPosition(default_mesh, 5, PLVector3(0, 0, -20));
  plSetMeshUniformColour(default_mesh, PLColour(255, 0, 0, 255));
  plSetMeshShaderProgram(default_mesh, Shaders_GetProgram(SHADER_GenericUntextured));
  plUploadMesh(default_mesh);
  fallback_ = plCreateBasicStaticModel(default_mesh);

  // todo: do something with this...
  CacheModelData();

#if 0
  model_cache.pigs[PIG_CLASS_ACE] = Model_LoadFile("chars/pigs/ac_hi", true);
  model_cache.pigs[PIG_CLASS_COMMANDO] = Model_LoadFile("chars/pigs/sb_hi", true);
  model_cache.pigs[PIG_CLASS_GRUNT] = Model_LoadFile("chars/pigs/gr_hi", true);
  model_cache.pigs[PIG_CLASS_HEAVY] = Model_LoadFile("chars/pigs/hv_hi", true);
  model_cache.pigs[PIG_CLASS_LEGEND] = Model_LoadFile("chars/pigs/le_hi", true);
  model_cache.pigs[PIG_CLASS_MEDIC] = Model_LoadFile("chars/pigs/me_hi", true);
  model_cache.pigs[PIG_CLASS_SABOTEUR] = Model_LoadFile("chars/pigs/sa_hi", true);
  model_cache.pigs[PIG_CLASS_SNIPER] = Model_LoadFile("chars/pigs/sn_hi", true);
  model_cache.pigs[PIG_CLASS_SPY] = Model_LoadFile("chars/pigs/sp_hi", true);

  /* debug loading min models */
  PLModel* debug = Model_LoadFile("chars/shed1d", false);
  if(debug != NULL) {
      plDestroyModel(default_model);
      default_model = debug;
  }
#endif
}

ModelManager::~ModelManager() {
  DestroyModels();

  plDestroyModel(fallback_);
}

void ModelManager::DestroyModels() {
  for (const auto &i : cached_models_) {
    plDestroyModel(i.second);
  }
  cached_models_.clear();
}

void ModelManager::DestroyModel(PLModel *model) {
  // Never EVER delete the default model!
  if (model == fallback_) {
    return;
  }

  for (const auto &i : cached_models_) {
    if (i.second == model) {
      plDestroyModel(i.second);
      cached_models_.erase(i.first);
      return;
    }
  }
}

PLModel *ModelManager::LoadModel(const std::string &path, bool abort_on_fail) {
  const char *fp = u_find2(path.c_str(), supported_model_formats, abort_on_fail);
  if (fp == nullptr) {
    return fallback_;
  }

  PLModel *model = plLoadModel(fp);
  if (model == nullptr) {
    if (abort_on_fail) {
      Error("Failed to load model, \"%s\", aborting (%s)!\n", fp, plGetError())
    }

    LogWarn("Failed to load model, \"%s\" (%s)!\n", fp, plGetError());
    return fallback_;
  }

  return model;
}

PLModel *ModelManager::LoadCachedModel(const std::string &path, bool abort_on_fail) {
  auto i = cached_models_.find(path);
  if (i != cached_models_.end()) {
    return i->second;
  }

  PLModel *model = LoadModel(path, abort_on_fail);
  cached_models_.emplace(path, model);
  return model;
}

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
#include <PL/platform_graphics_camera.h>

#include "engine.h"
#include "Map.h"
#include "model.h"
#include "mod_support.h"
#include "frontend.h"
#include "script/script.h"
#include "script/ScriptConfig.h"
#include "game/ActorManager.h"
#include "map_manager.h"

#include "graphics/display.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/texture_atlas.h"

#if 0
/* for now these are hard-coded, but
 * eventually we'll do this through a
 * script instead */
MapManifest map_descriptors[]={
        // Single-player

        //{"camp", "Boot camp", MAP_MODE_SINGLEPLAYER},
        {"estu", "The War Foundation", MAP_MODE_SINGLEPLAYER},
        {"road", "Routine Patrol", MAP_MODE_SINGLEPLAYER},
        {"trench", "Trench Warfare", MAP_MODE_SINGLEPLAYER},
        {"devi", "Morning Glory!", MAP_MODE_SINGLEPLAYER},
        {"rumble", "Island Invasion", MAP_MODE_SINGLEPLAYER},
        {"zulus", "Under Siege", MAP_MODE_SINGLEPLAYER},
        {"twin", "Communication Breakdown", MAP_MODE_SINGLEPLAYER},
        {"sniper", "The Spying Game", MAP_MODE_SINGLEPLAYER},
        {"mashed", "The Village People", MAP_MODE_SINGLEPLAYER},
        {"guns", "Bangers 'N' Mash", MAP_MODE_SINGLEPLAYER},
        {"liberate", "Saving Private Rind", MAP_MODE_SINGLEPLAYER},
        {"oasis", "Just Deserts", MAP_MODE_SINGLEPLAYER},
        {"fjords", "Glacier Guns", MAP_MODE_SINGLEPLAYER},
        {"eyrie", "Battle Stations", MAP_MODE_SINGLEPLAYER},
        {"bay", "Fortified Swine", MAP_MODE_SINGLEPLAYER},
        {"medix", "Over The Top", MAP_MODE_SINGLEPLAYER},
        {"bridge", "Geneva Convention", MAP_MODE_SINGLEPLAYER},
        {"desval", "I Spy...", MAP_MODE_SINGLEPLAYER},
        {"snake", "Chemical Compound", MAP_MODE_SINGLEPLAYER},
        {"emplace", "Achilles Heal", MAP_MODE_SINGLEPLAYER},
        {"supline", "High And Dry", MAP_MODE_SINGLEPLAYER},
        {"keep", "Assassination", MAP_MODE_SINGLEPLAYER},
        {"tester", "Hero Warship", MAP_MODE_SINGLEPLAYER},
        {"foot", "Hamburger Hill", MAP_MODE_SINGLEPLAYER},
        {"final", "Well, Well, Well!", MAP_MODE_SINGLEPLAYER},

        // Multi-player

        {"iceflow", "Ice-Flow", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        //{"archi", "You Hillock", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"dbowl", "Death Bowl", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"mlake", "Frost Fight", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"lake", "The Lake", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"ice", "Chill Hill", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"sepia1", "Square Off", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"oneway", "One Way System", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"ridge", "Ridge Back", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"island", "Island Hopper", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},

        {"play1", "Play Pen", MAP_MODE_SURVIVAL_NOVICE},
        {"play2", "Duvet Fun", MAP_MODE_SURVIVAL_NOVICE},
        {"lunar1", "Pigs in Space", MAP_MODE_SURVIVAL_NOVICE},
        {"hell3", "Skulduggery", MAP_MODE_SURVIVAL_NOVICE},
        {"hell2", "Pigin' Hell", MAP_MODE_SURVIVAL_NOVICE},
        {"creepy2", "Graveyard Shift", MAP_MODE_SURVIVAL_NOVICE},

        {"boom", "Friendly Fire", MAP_MODE_SURVIVAL_STRATEGY},
        {"lecprod", "Bridge The Gap", MAP_MODE_SURVIVAL_STRATEGY},
        {"bhill", "Dam Busters", MAP_MODE_SURVIVAL_STRATEGY},
        {"bute", "Moon Buttes", MAP_MODE_SURVIVAL_STRATEGY},
        {"maze", "Hedge Maze", MAP_MODE_SURVIVAL_STRATEGY},
        {"cmass", "Cratermass", MAP_MODE_SURVIVAL_STRATEGY},
        {"artgun", "Doomed", MAP_MODE_SURVIVAL_STRATEGY},
        {"dval", "Death Valley", MAP_MODE_SURVIVAL_STRATEGY},
        {"dval2", "Death Valley 2", MAP_MODE_SURVIVAL_STRATEGY},

        {"demo", NULL, 0},
        {"demo2", NULL, 0},
        {"easy", NULL, 0},

        {"genbrack", NULL, MAP_MODE_GENERATED},
        {"genchalk", NULL, MAP_MODE_GENERATED},
        {"gendesrt", NULL, MAP_MODE_GENERATED},
        {"genlava", NULL, MAP_MODE_GENERATED},
        {"genmud", NULL, MAP_MODE_GENERATED},
        {"gensnow", NULL, MAP_MODE_GENERATED},

        {"hillbase", NULL, 0},

        {"airaid", NULL, 0},
        {"canal", NULL, 0},
        {"climb", NULL, 0},
        {"faceoff", NULL, 0},
        {"hell1", NULL, 0},
        {"hiroad", NULL, 0},
        {"hwd", NULL, 0},
        {"newsnu", NULL, 0},
        {"pdrag", NULL, 0},
        {"quack", NULL, 0},
        {"tdd", NULL, 0},
};
#endif

Map::Map(const std::string &name) {
  LogDebug("Loading map, %s...\n", name.c_str());

  id_name_ = name;

  manifest_ = MapManager::GetInstance()->GetManifest(name);
  if (manifest_ == nullptr) {
    LogWarn("Failed to get map descriptor, \"%s\"\n", name.c_str());
  }

  std::string base_path = "maps/" + name + "/";
  std::string p = u_find(std::string(base_path + name + ".pmg").c_str());
  terrain_ = new Terrain(p, base_path + "tiles/");

  p = u_find(std::string(base_path + name + ".pog").c_str());
  if (!plFileExists(p.c_str())) {
    LogWarn("POG, \"%s\", doesn't exist!\n", p.c_str());
  } else {
    LoadSpawns(p);
  }

  LoadSky();

  UpdateSky();
  UpdateLighting();
}

Map::~Map() {
  ModelManager::GetInstance()->DestroyModel(sky_model_top_);
  ModelManager::GetInstance()->DestroyModel(sky_model_bottom_);

  delete terrain_;

  // gross GROSS; change the clear colour back!
  g_state.gfx.clear_colour = {0, 0, 0, 255};
}

void Map::LoadSky() {
  if(sky_model_top_ == nullptr) {
    sky_model_top_ = LoadSkyModel("skys/skydome");
  }

   if(sky_model_bottom_ == nullptr) {
    sky_model_bottom_ = LoadSkyModel("skys/skydomeu");
  }
}

PLModel* Map::LoadSkyModel(const std::string &path) {
  PLModel* model = ModelManager::GetInstance()->LoadModel(path, true);
  model->model_matrix = plTranslateMatrix4(PLVector3(TERRAIN_PIXEL_WIDTH / 2, 0, TERRAIN_PIXEL_WIDTH / 2));
  // Default skydome is smaller than the map, so we'll scale it
  model->model_matrix = plScaleMatrix4(model->model_matrix, PLVector3(5, 5, 5));

  PLModelLod *lod = plGetModelLodLevel(model, 0);
  if (lod == nullptr) {
    Error("Failed to get first lod for sky mesh!\n");
  }

  PLMesh *mesh = lod->meshes[0];
  // This is a really crap hardcoded limit, just to ensure it's what we're expecting
  if (mesh->num_verts != 257) {
    Error("Unexpected number of vertices for sky mesh! (%d vs 257)\n", mesh->num_verts);
  }

  return model;
}

void Map::UpdateSky() {
  UpdateSkyModel(sky_model_top_);
  UpdateSkyModel(sky_model_bottom_);
}

void Map::UpdateSkyModel(PLModel *model) {
  u_assert(model != nullptr, "attempted to apply sky colours prior to loading sky dome");

  PLModelLod *lod = plGetModelLodLevel(model, 0);
  if (lod == nullptr) {
    LogWarn("Failed to get first lod for sky mesh!\n");
    return;
  }

  PLMesh *mesh = lod->meshes[0];
  // Below is a PSX-style gradient sky implementation
  const unsigned int solid_steps = 3;
  const unsigned int grad_steps = 6;
  PLColour top = manifest_->sky_colour_top;
  PLColour bottom = manifest_->sky_colour_bottom;
  int stepr = ((int) (bottom.r) - (int) (top.r)) / (int) (grad_steps);
  int stepg = ((int) (bottom.g) - (int) (top.g)) / (int) (grad_steps);
  int stepb = ((int) (bottom.b) - (int) (top.b)) / (int) (grad_steps);

  if (stepr < 0) { stepr += 255; }
  if (stepg < 0) { stepg += 255; }
  if (stepb < 0) { stepb += 255; }

  PLColour colour = top;
  for (unsigned int i = 0, j = 31, s = 0; i < mesh->num_verts; ++i, ++j) {
    if (j == 32) {
      if (++s >= solid_steps) {
        colour.r += stepr;
        colour.g += stepg;
        colour.b += stepb;
      }
      j = 0;
    }

    mesh->vertices[i].colour = colour;
  }

  plUploadMesh(mesh);
}

void Map::UpdateLighting() {
  PLShaderProgram *program = Shaders_GetProgram(SHADER_GenericTexturedLit);
  if (program == nullptr) {
    return;
  }

  plSetNamedShaderUniformVector4(program, "fog_colour", manifest_->fog_colour.ToVec4());
  plSetNamedShaderUniformFloat(program, "fog_near", manifest_->fog_intensity);
  plSetNamedShaderUniformFloat(program, "fog_far", manifest_->fog_distance);

  PLVector3 sun_position(1.0f, -manifest_->sun_pitch, 0);
  PLMatrix4 sun_matrix =
      plMultiplyMatrix4(
          plTranslateMatrix4(sun_position),
          plRotateMatrix4(manifest_->sun_yaw, PLVector3(0, 1.0f, 0)));
  sun_position.x = sun_matrix.m[0];
  sun_position.z = sun_matrix.m[8];
#if 0
  debug_sun_position = sun_position;
#endif
  plSetNamedShaderUniformVector3(program, "sun_position", sun_position);
  plSetNamedShaderUniformVector4(program, "sun_colour", manifest_->sun_colour.ToVec4());

  plSetNamedShaderUniformVector4(program, "ambient_colour", manifest_->ambient_colour.ToVec4());
}

void Map::LoadSpawns(const std::string &path) {
  std::ifstream ifs(path, std::ios_base::in | std::ios_base::binary);
  if (!ifs.is_open()) {
    Error("Failed to open actor data, \"%s\", aborting!\n", path.c_str());
  }

  uint16_t num_indices;
  try {
    ifs.read(reinterpret_cast<char *>(&num_indices), sizeof(uint16_t));
  } catch (const std::ifstream::failure &err) {
    Error("Failed to read POG indices count, \"%s\", aborting!\n%s (%d)\n", err.what(), err.code().value());
  }

  spawns_.resize(num_indices);

  try {
    ifs.read(reinterpret_cast<char *>(spawns_.data()), sizeof(MapSpawn) * num_indices);
  } catch (const std::ifstream::failure &err) {
    Error("Failed to read POG spawns, \"%s\", aborting!\n%s (%d)\n", err.what(), err.code().value());
  }

  /* now we'll bump the coords here, to make life easy */
  for (auto &spawn : spawns_) {
    spawn.position[0] += (TERRAIN_PIXEL_WIDTH / 2);
    spawn.position[2] *= -1;
    spawn.position[2] += (TERRAIN_PIXEL_WIDTH / 2);
  }
}

void Map::Draw() {
  Shaders_SetProgram(SHADER_GenericUntextured);
  plDrawModel(sky_model_top_);
  plDrawModel(sky_model_bottom_);

  terrain_->Draw();

#if 0 // debug sun position
  Shaders_SetProgram(SHADER_GenericUntextured);
  PLModel *sprite = ModelManager::GetInstance()->GetFallbackModel();
  PLMesh *mesh = sprite->levels[0].meshes[0];
  plSetMeshUniformColour(mesh, PLColour(0, 255, 0, 255));
  sprite->model_matrix = plTranslateMatrix4(debug_sun_position);
  plDrawModel(sprite);
  plSetMeshUniformColour(mesh, PLColour(255, 255, 0, 255));
  sprite->model_matrix = plTranslateMatrix4(PLVector3(0, 0, 0));
  plDrawModel(sprite);
  plSetMeshUniformColour(mesh, PLColour(255, 0, 0, 255));
#endif
}

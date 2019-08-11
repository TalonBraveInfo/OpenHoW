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

#include <list>
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

//Precalculated vertices for chunk rendering
//TODO: Share one index buffer instance between all chunks
const static unsigned int chunkIndices[96] = {
    0, 2, 1, 1, 2, 3,
    4, 6, 5, 5, 6, 7,
    8, 10, 9, 9, 10, 11,
    12, 14, 13, 13, 14, 15,
    16, 18, 17, 17, 18, 19,
    20, 22, 21, 21, 22, 23,
    24, 26, 25, 25, 26, 27,
    28, 30, 29, 29, 30, 31,
    32, 34, 33, 33, 34, 35,
    36, 38, 37, 37, 38, 39,
    40, 42, 41, 41, 42, 43,
    44, 46, 45, 45, 46, 47,
    48, 50, 49, 49, 50, 51,
    52, 54, 53, 53, 54, 55,
    56, 58, 57, 57, 58, 59,
    60, 62, 61, 61, 62, 63,
};

Map::Map(const std::string &name) : Map() {
  LogDebug("Loading map, %s...\n", name.c_str());

  id_name_ = name;

  manifest_ = MapManager::GetInstance()->GetManifest(name);
  if (manifest_ == nullptr) {
    LogWarn("Failed to get map descriptor, \"%s\"\n", name.c_str());
  }

#if 0
  if(mode.game_mode != MAP_MODE_EDITOR && !(desc->flags & mode.game_mode)) {
      throw std::runtime_error("This mode is unsupported by this map, " + std::string(mode.map) + ", aborting!\n");
  }
#endif

  std::string base_path = "maps/" + name + "/";
  std::string p = u_find(std::string(base_path + name + ".pmg").c_str());
  if (!plFileExists(p.c_str())) {
    throw std::runtime_error("PMG, " + p + ", doesn't exist!\n");
  }

  LoadTiles(p);

  p = u_find(std::string(base_path + name + ".pog").c_str());
  if (!plFileExists(p.c_str())) {
    LogWarn("POG, \"%s\", doesn't exist!\n", p.c_str());
  } else {
    LoadSpawns(p);
  }

  LoadSky();

  UpdateSky();
  UpdateLighting();

  GenerateOverview();
}

Map::Map() {
  chunks_.resize(MAP_CHUNKS);
}

Map::~Map() {
  delete texture_atlas_;

  plDestroyTexture(overview_, true);

  for (auto &chunk : chunks_) {
    plDestroyModel(chunk.model);
  }

  ModelManager::GetInstance()->DestroyModel(sky_model_);

  // gross GROSS; change the clear colour back!
  g_state.gfx.clear_colour = {0, 0, 0, 255};
}

MapChunk *Map::GetChunk(const PLVector2 &pos) {
  if (pos.x < 0 || std::floor(pos.x) >= MAP_PIXEL_WIDTH ||
      pos.y < 0 || std::floor(pos.y) >= MAP_PIXEL_WIDTH) {
    return nullptr;
  }

  uint idx = ((uint) (pos.x) / MAP_CHUNK_PIXEL_WIDTH) + (((uint) (pos.y) / MAP_CHUNK_PIXEL_WIDTH) * MAP_CHUNK_ROW);
  if (idx >= chunks_.size()) {
    LogWarn("Attempted to get an out of bounds chunk index!\n");
    return nullptr;
  }

  return &chunks_[idx];
}

MapTile *Map::GetTile(const PLVector2 &pos) {
  MapChunk *chunk = GetChunk(pos);
  if (chunk == nullptr) {
    return nullptr;
  }

  uint idx = (((uint) (pos.x) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) +
      ((((uint) (pos.y) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) * MAP_CHUNK_ROW_TILES);
  if (idx >= MAP_CHUNK_TILES) {
    LogWarn("Attempted to get an out of bounds tile index!\n");
    return nullptr;
  }

  return &chunk->tiles[idx];
}

float Map::GetHeight(const PLVector2 &pos) {
  MapTile *tile = GetTile(pos);
  if (tile == nullptr) {
    return 0;
  }

  float tile_x = pos.x - std::floor(pos.x);
  float tile_y = pos.y - std::floor(pos.y);

  float x = tile->height[0] + ((tile->height[1] - tile->height[0]) * tile_x);
  float y = tile->height[2] + ((tile->height[3] - tile->height[2]) * tile_x);
  float z = x + ((y - x) * tile_y);

  return z;
}

void Map::LoadSky() {
  if (sky_model_ == nullptr) {
    sky_model_ = ModelManager::GetInstance()->LoadModel("skys/skydome", true);
    sky_model_->model_matrix =
        plTranslateMatrix4(PLVector3(MAP_PIXEL_WIDTH / 2, 0, MAP_PIXEL_WIDTH / 2));
    // Default skydome is smaller than the map, so we'll scale it
    sky_model_->model_matrix = plScaleMatrix4(sky_model_->model_matrix, PLVector3(5, 5, 5));

    PLModelLod *lod = plGetModelLodLevel(sky_model_, 0);
    if (lod == nullptr) {
      Error("Failed to get first lod for sky mesh!\n");
    }

    PLMesh *mesh = lod->meshes[0];
    // This is a really crap hardcoded limit, just to ensure it's what we're expecting
    if (mesh->num_verts != 257) {
      Error("Unexpected number of vertices for sky mesh! (%d vs 257)\n", mesh->num_verts);
    }
  }
}

void Map::UpdateSky() {
  u_assert(sky_model_ != nullptr, "attempted to apply sky colours prior to loading sky dome");

  PLModelLod *lod = plGetModelLodLevel(sky_model_, 0);
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

  // gross GROSS; ensures that the dome transitions out nicely
  g_state.gfx.clear_colour = bottom;
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
    spawn.position[0] += (MAP_PIXEL_WIDTH / 2);
    spawn.position[2] *= -1;
    spawn.position[2] += (MAP_PIXEL_WIDTH / 2);
  }
}

void Map::LoadTiles(const std::string &path) {
  FILE *fh = std::fopen(path.c_str(), "rb");
  if (fh == nullptr) {
    Error("Failed to open tile data, \"%s\", aborting\n", path.c_str());
  }

  chunks_.resize(MAP_CHUNKS);

  // todo: move into map constructor (see map-edit branch)
  texture_atlas_ = new TextureAtlas(512, 8);

  for (unsigned int chunk_y = 0; chunk_y < MAP_CHUNK_ROW; ++chunk_y) {
    for (unsigned int chunk_x = 0; chunk_x < MAP_CHUNK_ROW; ++chunk_x) {
      MapChunk &current_chunk = chunks_[chunk_x + chunk_y * MAP_CHUNK_ROW];

      struct __attribute__((packed)) {
        /* offsets */
        uint16_t x{0};
        uint16_t y{0};
        uint16_t z{0};

        uint16_t unknown0{0};
      } chunk;
      if (std::fread(&chunk, sizeof(chunk), 1, fh) != 1) {
        Error("unexpected end of file, aborting!\n");
      }

      current_chunk.offset = PLVector3(static_cast<float>(chunk_x), static_cast<float>(chunk_y), 0);

      struct __attribute__((packed)) {
        int16_t height{0};
        uint16_t lighting{0};
      } vertices[25];
      if (std::fread(vertices, sizeof(*vertices), 25, fh) != 25) {
        Error("Unexpected end of file, aborting!\n");
      }

      // Find the maximum and minimum points
      for (auto &vertex : vertices) {
        if (vertex.height > max_height_) {
          max_height_ = vertex.height;
        }
        if (vertex.height < min_height_) {
          min_height_ = vertex.height;
        }
      }

      std::fseek(fh, 4, SEEK_CUR);

      for (unsigned int tile_y = 0; tile_y < MAP_CHUNK_ROW_TILES; ++tile_y) {
        for (unsigned int tile_x = 0; tile_x < MAP_CHUNK_ROW_TILES; ++tile_x) {
          struct __attribute__((packed)) {
            int8_t unused0[6]{0, 0, 0, 0, 0, 0};
            uint8_t type{0};
            uint8_t slip{0};
            int16_t unused1{0};
            uint8_t rotation{0};
            uint32_t texture{0};
            uint8_t unused2{0};
          } tile;
          if (std::fread(&tile, sizeof(tile), 1, fh) != 1) {
            Error("unexpected end of file, aborting!\n");
          }

          MapTile *current_tile = &current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES];
          current_tile->type = (tile.type & 31U);
          current_tile->flags = (tile.type & ~31U);
          current_tile->flip = tile.rotation;
          current_tile->slip = 0;
          current_tile->texture = std::to_string(tile.texture);

          texture_atlas_->AddImage("maps/" + id_name_ + "/tiles/" + current_tile->texture);

          current_tile->height[0] = vertices[(tile_y * 5) + tile_x].height;
          current_tile->height[1] = vertices[(tile_y * 5) + tile_x + 1].height;
          current_tile->height[2] = vertices[((tile_y + 1) * 5) + tile_x].height;
          current_tile->height[3] = vertices[((tile_y + 1) * 5) + tile_x + 1].height;

          current_tile->shading[0] = vertices[(tile_y * 5) + tile_x].lighting;
          current_tile->shading[1] = vertices[(tile_y * 5) + tile_x + 1].lighting;
          current_tile->shading[2] = vertices[((tile_y + 1) * 5) + tile_x].lighting;
          current_tile->shading[3] = vertices[((tile_y + 1) * 5) + tile_x + 1].lighting;

          u_assert(current_chunk.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].type < MAX_TILE_TYPES,
                   "invalid tile type!\n");
        }
      }
    }
  }

  std::fclose(fh);

  texture_atlas_->Finalize();

  GenerateModels();
}

void Map::GenerateModels() {
  for (auto &chunk : chunks_) {
    GenerateModel(&chunk);
  }

  std::list<PLMesh*> meshes;
  for (auto &chunk : chunks_) {
    PLModelLod *lod = plGetModelLodLevel(chunk.model, 0);
    meshes.push_back(lod->meshes[0]);
  }

  Mesh_GenerateFragmentedMeshNormals(meshes);
}

void Map::GenerateModel(MapChunk *chunk) {
  if (chunk->model != nullptr) {
    plDestroyModel(chunk->model);
    chunk->model = nullptr;
  }

  PLMesh *chunk_mesh = plCreateMeshInit(PL_MESH_TRIANGLES, PL_DRAW_DYNAMIC, 32, 64, (void *) chunkIndices, nullptr);
  if (chunk_mesh == nullptr) {
    Error("Unable to create map chunk mesh, aborting (%s)!\n", plGetError());
  }

  int cm_idx = 0;
  for (unsigned int tile_y = 0; tile_y < MAP_CHUNK_ROW_TILES; ++tile_y) {
    for (unsigned int tile_x = 0; tile_x < MAP_CHUNK_ROW_TILES; ++tile_x) {
      const MapTile *current_tile = &chunk->tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES];

      float tx_x, tx_y, tx_w, tx_h;
      texture_atlas_->GetTextureCoords(current_tile->texture, &tx_x, &tx_y, &tx_w, &tx_h);

      /* MAP_FLIP_FLAG_X flips around texture sheet coords, not map coords. */
      if (current_tile->flip & MAP_FLIP_FLAG_X) {
        tx_x = tx_x + tx_w;
        tx_w = -tx_w;
      }

      /* ST coords for each corner of the tile. */
      float tx_Ax[] = {tx_x, tx_x + tx_w, tx_x, tx_x + tx_w};
      float tx_Ay[] = {tx_y, tx_y, tx_y + tx_h, tx_y + tx_h};

      /* Rotate a quad of ST coords 90 degrees clockwise. */
      auto rot90 = [](float *x) {
        float c = x[0];
        x[0] = x[2];
        x[2] = x[3];
        x[3] = x[1];
        x[1] = c;
      };

      if (current_tile->flip & MAP_FLIP_FLAG_ROTATE_90) {
        rot90(tx_Ax);
        rot90(tx_Ay);
      }

      if (current_tile->flip & MAP_FLIP_FLAG_ROTATE_180) {
        rot90(tx_Ax);
        rot90(tx_Ay);
        rot90(tx_Ax);
        rot90(tx_Ay);
      }

      /* MAP_FLIP_FLAG_ROTATE_270 is implemented by ORing 90 and 180 together. */

      for (int i = 0; i < 4; ++i, ++cm_idx) {
        float x = (chunk->offset.x * MAP_CHUNK_PIXEL_WIDTH) + (tile_x + (i % 2)) * MAP_TILE_PIXEL_WIDTH;
        float z = (chunk->offset.y * MAP_CHUNK_PIXEL_WIDTH) + (tile_y + (i / 2)) * MAP_TILE_PIXEL_WIDTH;
        plSetMeshVertexST(chunk_mesh, cm_idx, tx_Ax[i], tx_Ay[i]);
        plSetMeshVertexPosition(chunk_mesh, cm_idx, PLVector3(x, current_tile->height[i], z));
        plSetMeshVertexColour(chunk_mesh, cm_idx, PLColour(
            current_tile->shading[i],
            current_tile->shading[i],
            current_tile->shading[i]));
      }
    }
  }

  chunk_mesh->texture = texture_atlas_->GetTexture();

  // attach the mesh to our model
  PLModel *model = plCreateBasicStaticModel(chunk_mesh);
  if (model == nullptr) {
    Error("Failed to create map model (%s), aborting!\n", plGetError());
  }

  snprintf(model->name, sizeof(model->name), "map_chunk_%d_%d",
      static_cast<int>(chunk->offset.x),
      static_cast<int>(chunk->offset.y));

  chunk->model = model;
}

void Map::GenerateOverview() {
  static const PLColour colours[MAX_TILE_TYPES] = {
      {60, 50, 40},     // Mud
      {40, 70, 40},     // Grass
      {128, 128, 128},  // Metal
      {153, 94, 34},    // Wood
      {90, 90, 150},    // Water
      {50, 50, 50},     // Stone
      {50, 50, 50},     // Rock
      {100, 80, 30},    // Sand
      {180, 240, 240},  // Ice
      {100, 100, 100},  // Snow
      {60, 50, 40},     // Quagmire
      {100, 240, 53}    // Lava/Poison
  };

  // Create our storage
  PLImage *image = plCreateImage(nullptr, 64, 64, PL_COLOURFORMAT_RGB, PL_IMAGEFORMAT_RGB8);

  // Now write into the image buffer
  uint8_t *buf = image->data[0];
  for (uint8_t y = 0; y < 64; ++y) {
    for (uint8_t x = 0; x < 64; ++x) {
      PLVector2 position(x * (MAP_PIXEL_WIDTH / 64), y * (MAP_PIXEL_WIDTH / 64));
      MapTile *tile = GetTile(position);
      u_assert(tile != nullptr, "Hit an invalid tile during overview generation!\n");

      auto mod = static_cast<int>((GetHeight(position) + ((GetMaxHeight() + GetMinHeight()) / 2)) / 255);
      PLColour rgb = PLColour(
          std::min((colours[tile->type].r / 9) * mod, 255),
          std::min((colours[tile->type].g / 9) * mod, 255),
          std::min((colours[tile->type].b / 9) * mod, 255)
      );
      if (tile->flags & TILE_FLAG_MINE) {
        rgb = PLColour(255, 0, 0);
      }

      *(buf++) = rgb.r;
      *(buf++) = rgb.g;
      *(buf++) = rgb.b;
    }
  }

#ifdef _DEBUG
  if (plCreatePath("./debug/generated/")) {
    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path) - 1, "./debug/generated/%dx%d_%s.png",
             image->width, image->height, id_name_.c_str());
    plWriteImage(image, path);
  }
#endif

  // Allow rebuilding overview texture
  plDestroyTexture(overview_, true);

  if ((overview_ = plCreateTexture()) == nullptr) {
    Error("Failed to generate overview texture slot!\n%s\n", plGetError());
  }

  plUploadTextureImage(overview_, image);
  plDestroyImage(image);
}

void Map::Draw() {
  Shaders_SetProgram(SHADER_GenericUntextured);
  plDrawModel(sky_model_);

  Shaders_SetProgram(SHADER_GenericTexturedLit);
  g_state.gfx.num_chunks_drawn = 0;
  for (const auto &chunk : chunks_) {
    g_state.gfx.num_chunks_drawn++;
    plDrawModel(chunk.model);
  }

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

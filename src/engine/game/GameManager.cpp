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

#include "../engine.h"
#include "../frontend.h"
#include "../audio/audio.h"
#include "../Map.h"

#include "GameManager.h"
#include "ActorManager.h"
#include "mode_base.h"
#include "../model.h"
#include "../script/ScriptConfig.h"

GameManager* GameManager::instance_ = nullptr;

GameManager::GameManager() {
  plRegisterConsoleCommand("map", MapCommand, "");
  plRegisterConsoleCommand("maps", MapsCommand, "");
}

GameManager::~GameManager() {
  map_manifests_.clear();
}

void GameManager::Tick() {
  FrontEnd_Tick();

  if (active_mode_ == nullptr) {
    return;
  }

  if (ambient_emit_delay_ < g_state.sim_ticks) {
    const AudioSample* sample = ambient_samples_[rand() % MAX_AMBIENT_SAMPLES];
    if (sample != nullptr) {
      PLVector3 position = {
          plGenerateRandomf(TERRAIN_PIXEL_WIDTH),
          active_map_->GetTerrain()->GetMaxHeight(),
          plGenerateRandomf(TERRAIN_PIXEL_WIDTH)
      };
      AudioManager::GetInstance()->PlayLocalSound(sample, position, {0, 0, 0}, true, 0.5f);
    }

    ambient_emit_delay_ = g_state.sim_ticks + TICKS_PER_SECOND + rand() % (7 * TICKS_PER_SECOND);
  }

  active_mode_->Tick();
}

void GameManager::LoadMap(const std::string& name) {
  //FrontEnd_SetState(FE_MODE_LOADING);

  Map* map;
  try {
    map = new Map(name);
  } catch (const std::runtime_error& e) {
    LogWarn("Failed to load map, aborting!\n%s\n", e.what());
    return;
  }

  if (active_map_ != nullptr) {
    ActorManager::GetInstance()->DestroyActors();
    ModelManager::GetInstance()->DestroyModels();
    delete active_map_;
  }

  active_map_ = map;

  MapManifest* manifest = GetMapManifest(name);
  std::string sample_ext = "d";
  if (manifest->time != "day") {
    sample_ext = "n";
  }

  for (unsigned int i = 1, idx = 0; i < 4; ++i) {
    if (i < 3) {
      ambient_samples_[idx++] = AudioManager::GetInstance()->CacheSample(
          "audio/amb_" + std::to_string(i) + sample_ext + ".wav", false);
    }
    ambient_samples_[idx++] =
        AudioManager::GetInstance()->CacheSample("audio/batt_s" + std::to_string(i) + ".wav", false);
    ambient_samples_[idx++] =
        AudioManager::GetInstance()->CacheSample("audio/batt_l" + std::to_string(i) + ".wav", false);
  }

  ambient_emit_delay_ = g_state.sim_ticks + rand() % 100;

  active_mode_ = new BaseGameMode();
  // call StartRound; deals with spawning everything in and other mode specific logic
  active_mode_->StartRound();

  /* todo: we should actually pause here and wait for user input
   *       otherwise players won't have time to read the loading screen */
  FrontEnd_SetState(FE_MODE_GAME);
}

void GameManager::UnloadMap() {
  for (auto& ambient_sample : ambient_samples_) {
    delete ambient_sample;
  }

  delete active_mode_;
  delete active_map_;
}

void GameManager::RegisterMapManifest(const std::string& path) {
  LogInfo("Registering map \"%s\"...\n", path.c_str());

  MapManifest manifest;
  manifest.path = path;
  try {
    ScriptConfig config(path);
    manifest.name = config.GetStringProperty("name", manifest.name);
    manifest.author = config.GetStringProperty("author", manifest.author);
    manifest.description = config.GetStringProperty("description", manifest.description);
    manifest.modes = config.GetArrayStrings("modes");
    manifest.ambient_colour = config.GetColourProperty("ambientColour", manifest.ambient_colour);
    manifest.sky_colour_top = config.GetColourProperty("skyColourTop", manifest.sky_colour_top);
    manifest.sky_colour_bottom = config.GetColourProperty("skyColourBottom", manifest.sky_colour_bottom);
    manifest.sun_colour = config.GetColourProperty("sunColour", manifest.sun_colour);
    manifest.sun_yaw = config.GetFloatProperty("sunYaw", manifest.sun_yaw);
    manifest.sun_pitch = config.GetFloatProperty("sunPitch", manifest.sun_pitch);
    manifest.temperature = config.GetStringProperty("temperature", manifest.temperature);
    manifest.weather = config.GetStringProperty("weather", manifest.weather);
    manifest.time = config.GetStringProperty("time", manifest.time);

    // Fog
    manifest.fog_colour = config.GetColourProperty("fogColour", manifest.fog_colour);
    manifest.fog_intensity = config.GetFloatProperty("fogIntensity", manifest.fog_intensity);
    manifest.fog_distance = config.GetFloatProperty("fogDistance", manifest.fog_distance);
  } catch (const std::exception& e) {
    LogWarn("Failed to read map config, \"%s\"!\n%s\n", path.c_str(), e.what());
  }

  char temp_buf[64];
  plStripExtension(temp_buf, sizeof(temp_buf), plGetFileName(path.c_str()));

  map_manifests_.insert(std::make_pair(temp_buf, manifest));
}

static void RegisterManifestInterface(const char* path) {
  GameManager::GetInstance()->RegisterMapManifest(path);
}

void GameManager::RegisterMapManifests() {
  map_manifests_.clear();

  std::string scan_path = std::string(u_get_base_path()) + "/campaigns/" + u_get_mod_path() + "/maps";
  plScanDirectory(scan_path.c_str(), "map", RegisterManifestInterface, false);
}

MapManifest* GameManager::GetMapManifest(const std::string& name) {
  auto manifest = map_manifests_.find(name);
  if (manifest != map_manifests_.end()) {
    return &manifest->second;
  }

  LogWarn("Failed to get manifest for \"%s\"!\n", name.c_str());

  static MapManifest default_descript;
  return &default_descript;
}

void GameManager::MapCommand(unsigned int argc, char **argv) {
  if (argc < 2) {
    LogWarn("Invalid number of arguments, ignoring!\n");
    return;
  }

  std::string mode = "singleplayer";
  const MapManifest *desc = GetInstance()->GetMapManifest(argv[1]);
  if (desc != nullptr && !desc->modes.empty()) {
    mode = desc->modes[0];
  }

  GetInstance()->LoadMap(argv[1]);
}

void GameManager::MapsCommand(unsigned int argc, char **argv) {
  if (GetInstance()->map_manifests_.empty()) {
    LogWarn("No maps available!\n");
    return;
  }

  for (auto manifest : GetInstance()->map_manifests_) {
    MapManifest *desc = &manifest.second;
    std::string out =
        desc->name + "/" + manifest.first +
            " : " + desc->description +
            " :";
    // print out all the supported modes
    for (size_t i = 0; i < desc->modes.size(); ++i) {
      out += (i == 0 ? " " : ", ") + desc->modes[i];
    }
    LogInfo("%s\n", out.c_str());
  }

  LogInfo("%u maps\n", GetInstance()->map_manifests_.size());
}

Player* GameManager::GetCurrentPlayer() {
  if (active_mode_ == nullptr) {
    // todo: return local player... ?
    return nullptr;
  }

  return active_mode_->GetCurrentPlayer();
}

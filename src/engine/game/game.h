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

#include "TempGame.h"
#include "actors/actor.h"
#include "mode_interface.h"
#include "../graphics/camera.h"

struct Team {
  std::string name{"none"};
  std::string description{"none"};
  std::string identifier{"en"};

  // data directories
  std::string pig_textures{"chars/pigs/british"};
  std::string paper_texture{"frontend/papers/british"};
  std::string debrief_texture{"frontend/debrief/unifeng"};

  std::string Serialize() { /* TODO */ return ""; }
};

class Player {
 public:
  Player();
  ~Player();

  void PossessChild(unsigned int index);
  void DepossessChild();

  void SetControllerSlot(unsigned int slot) { input_slot = slot; }
  unsigned int GetControllerSlot() { return input_slot; }

  const Team* GetTeam() const { return team_; }
  void SetTeam(const Team* team) { team_ = team; }

 protected:
 private:
  std::string name_{"none"};

  unsigned int input_slot{0}; // Controller slot
  unsigned int identity{}; // Identity of the team (e.g. Tommy etc.)

  const Team* team_{nullptr};

  unsigned int current_target_{0};
};

struct MapManifest {
  std::string filepath; // path to manifest
  std::string filename; // name of the manifest

  std::string name{"none"};                       // 'BOOT CAMP'
  std::string author{"none"};                     // creator of the map
  std::string description{"none"};                //
  std::string tile_directory;
  std::vector<std::string> modes;                 // supported gameplay types
  PLColour ambient_colour{255, 255, 255, 255};    // ambient colour
  // Sky gradient
  PLColour sky_colour_top{0, 104, 156};
  PLColour sky_colour_bottom{223, 255, 255};
  // Sun/lighting properties
  PLColour sun_colour{255, 255, 255};      // directional colour
  float sun_yaw{0}, sun_pitch{0};       // light direction (yaw/angle)
  // Fog
  PLColour fog_colour{223, 255, 255, 255};
  float fog_intensity{30.0f};
  float fog_distance{100.0f};
  // Misc
  std::string temperature{"normal"};       // Determines idle animation set. Can be normal/hot/cold
  std::string time{"day"};              // Determines ambient sound set. Can be day/night
  std::string weather{"clear"};         // Determines weather particles. Can be clear/rain/snow

  std::string Serialize();
};

class Map;

class GameManager {
 private:
  GameManager();
  ~GameManager();

 public:
  void Tick();

  Camera* GetCamera() { return camera_; }

  // Map

  void LoadMap(const std::string& name);
  void UnloadMap();

  void RegisterMapManifest(const std::string& path);
  void RegisterMapManifests();

  typedef std::map<std::string, MapManifest> MapManifestMap;
  MapManifest* GetMapManifest(const std::string& name);
  const MapManifestMap& GetMapManifests() { return map_manifests_; };
  MapManifest* CreateManifest(const std::string& name);
  void SaveManifest(const std::string& name, const MapManifest& manifest);

  Map* GetCurrentMap() { return active_map_; }

  IGameMode* GetMode() { return active_mode_; }

  void EndCurrentMode();

  // Player Handling

 protected:
 private:
  static void MapCommand(unsigned int argc, char* argv[]);
  static void CreateMapCommand(unsigned int argc, char* argv[]);
  static void MapsCommand(unsigned int argc, char* argv[]);
  static void GiveItemCommand(unsigned int argc, char* argv[]);

  Actor* active_actor_{nullptr};

  Camera* camera_{nullptr};

  IGameMode* active_mode_{nullptr};
  Map* active_map_{nullptr};

  Actor* current_actor_{nullptr};

  std::map<std::string, MapManifest> map_manifests_;

  std::vector<Player> players_;
  std::vector<Team> teams_;

#define MAX_AMBIENT_SAMPLES 8
  double ambient_emit_delay_{0};
  const struct AudioSample* ambient_samples_[MAX_AMBIENT_SAMPLES]{};

  friend class openhow::Engine;
};

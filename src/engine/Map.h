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

#include "map_manager.h"
#include "terrain.h"

struct ActorSpawn {
  std::string class_name;
  PLVector3 position; // position in the world
  uint16_t index;                  // todo
  PLVector3 angles; // angles in the world
  uint16_t type;                   // todo

  int16_t bounds[3]; // collision bounds
  uint16_t bounds_type; // box, prism, sphere and none

  int16_t energy;
  uint8_t appearance;
  uint8_t team; // uk, usa, german, french, japanese, soviet

  uint16_t objective;
  uint8_t objective_actor_id;
  uint8_t objective_extra[2];

  PLVector3 fallback_position;

  int16_t extra;
  ActorSpawn* attachment{nullptr};
};

struct MapManifest;

/* end format data */

class Map {
 public:
  explicit Map(const std::string& name);
  ~Map();

  void Draw();

  const std::string& GetId() { return id_name_; }
  MapManifest* GetManifest() { return manifest_; }
  Terrain* GetTerrain() { return terrain_; }

  const std::vector<ActorSpawn>& GetSpawns() { return spawns_; }

  void UpdateSky();

  void UpdateLighting();

 protected:
 private:
  void LoadSpawns(const std::string& path);
  void LoadSky();
  static PLModel* LoadSkyModel(const std::string& path);

  void UpdateSkyModel(PLModel* model);

  MapManifest* manifest_{nullptr};

  std::vector<ActorSpawn> spawns_;

  std::string id_name_;

  PLModel* sky_model_top_{nullptr};
  PLModel* sky_model_bottom_{nullptr};

  Terrain* terrain_{nullptr};
};

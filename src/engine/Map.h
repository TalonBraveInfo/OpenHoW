/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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

#include "terrain.h"

struct MapManifest;

/* end format data */

struct ActorSpawn;

class Map {
 public:
  explicit Map(MapManifest* manifest);
  ~Map();

  void Draw();

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

  PLModel* sky_model_top_{nullptr};
  PLModel* sky_model_bottom_{nullptr};

  Terrain* terrain_{nullptr};
};

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

struct MapManifest {
  std::string path;                               // path to manifest
  std::string name{"none"};                       // 'BOOT CAMP'
  std::string author{"none"};                     // creator of the map
  std::string description{"none"};                //
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
  std::string temperature{"hot"};       // Determines idle animation set. Can be normal/hot/cold
  std::string time{"day"};              // Determines ambient sound set. Can be day/night
  std::string weather{"clear"};         // Determines weather particles. Can be clear/rain/snow
};

class MapManager {
 public:
  static MapManager *GetInstance() {
    if (instance_ == nullptr) {
      instance_ = new MapManager();
    }

    return instance_;
  }

  static void DestroyInstance() {
    delete instance_;
    instance_ = nullptr;
  }

  void RegisterManifest(const std::string &path);
  void RegisterManifests();

  MapManifest *GetManifest(const std::string &name);
  const std::map<std::string, MapManifest> &GetManifests() { return manifests_; };

 protected:
 private:
  MapManager();
  ~MapManager();

  static void MapCommand(unsigned int argc, char *argv[]);
  static void MapsCommand(unsigned int argc, char *argv[]);

  std::map<std::string, MapManifest> manifests_;

  static MapManager *instance_;
};

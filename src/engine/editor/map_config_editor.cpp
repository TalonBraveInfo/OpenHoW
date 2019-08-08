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
#include "../game/GameManager.h"

#include "map_config_editor.h"

MapConfigEditor::MapConfigEditor() {
  map_ = GameManager::GetInstance()->GetCurrentMap();
  if (map_ == nullptr) {
    throw std::runtime_error("Attempted to create config editor without a valid map loaded!\n");
  }

  manifest_ = map_->GetManifest();
  backup_ = *manifest_;

  strncpy(name_buffer, manifest_->name.c_str(), sizeof(name_buffer));
  strncpy(author_buffer, manifest_->author.c_str(), sizeof(author_buffer));
}

MapConfigEditor::~MapConfigEditor() {
  RestoreManifest();
}

void MapConfigEditor::DisplayTemperature() {
  enum {
    TEMP_NORMAL, TEMP_HOT, TEMP_COLD, MAX_TEMP
  };
  static const char *temperatures[MAX_TEMP] = {"Normal", "Hot", "Cold"};
  static int temperature_index = -1;
  if (temperature_index == -1) {
    if (pl_strcasecmp(manifest_->temperature.c_str(), "hot") == 0) {
      temperature_index = TEMP_HOT;
    } else if (pl_strcasecmp(manifest_->temperature.c_str(), "cold") == 0) {
      temperature_index = TEMP_COLD;
    } else {
      temperature_index = TEMP_NORMAL;
    }
  }

  if (ImGui::BeginCombo("Temperature", temperatures[temperature_index])) {
    for (int i = 0; i < MAX_TEMP; ++i) {
      if (ImGui::Selectable(temperatures[i], (temperature_index == i))) {
        ImGui::SetItemDefaultFocus();
        temperature_index = i;
        manifest_->temperature = u_stringtolower(temperatures[temperature_index]);
      }
    }

    ImGui::EndCombo();
  }
}

void MapConfigEditor::DisplayWeather() {
  enum {
    WEATHER_CLEAR, WEATHER_RAIN, WEATHER_SNOW, MAX_WEATHER
  };
  static const char *weather_labels[MAX_WEATHER] = {"Clear", "Rain", "Snow"};
  static int weather_index = -1;
  if (weather_index == -1) {
    if (pl_strcasecmp(manifest_->weather.c_str(), "rain") == 0) {
      weather_index = WEATHER_RAIN;
    } else if (pl_strcasecmp(manifest_->weather.c_str(), "snow") == 0) {
      weather_index = WEATHER_SNOW;
    } else {
      weather_index = WEATHER_CLEAR;
    }
  }

  if (ImGui::BeginCombo("Weather", weather_labels[weather_index])) {
    for (int i = 0; i < MAX_WEATHER; ++i) {
      if (ImGui::Selectable(weather_labels[i], (weather_index == i))) {
        ImGui::SetItemDefaultFocus();
        weather_index = i;
        manifest_->weather = u_stringtolower(weather_labels[weather_index]);
      }
    }

    ImGui::EndCombo();
  }
}

void MapConfigEditor::DisplayTime() {
  enum {
    TIME_DAY, TIME_NIGHT, MAX_TIME
  };
  static const char *times[MAX_TIME] = {"Day", "Night"};
  static int time_index = -1;
  if (time_index == -1) {
    if (pl_strcasecmp(manifest_->time.c_str(), "day") == 0) {
      time_index = TIME_DAY;
    } else {
      time_index = TIME_NIGHT;
    }
  }

  if (ImGui::BeginCombo("Time", times[time_index])) {
    for (int i = 0; i < MAX_TIME; ++i) {
      if (ImGui::Selectable(times[i], (time_index == i))) {
        ImGui::SetItemDefaultFocus();
        time_index = i;
        manifest_->time = u_stringtolower(times[time_index]);
      }
    }

    ImGui::EndCombo();
  }
}

void MapConfigEditor::Display() {
  ImGui::SetNextWindowSize(ImVec2(310, 512), ImGuiCond_Once);
  ImGui::Begin(dname("Map Config Editor"), &status_, ED_DEFAULT_WINDOW_FLAGS);
  ImGui::InputText("Name", name_buffer, sizeof(name_buffer));
  ImGui::InputText("Author", author_buffer, sizeof(author_buffer));

  // todo: mode selection - need to query wherever this ends up being implemented...

  ImGui::Separator();

  DisplayTemperature();
  DisplayWeather();
  DisplayTime();

  ImGui::Separator();

  ImGui::Text("Sky Settings");

  float rgb[3];
  rgb[0] = plByteToFloat(manifest_->sky_colour_top.r);
  rgb[1] = plByteToFloat(manifest_->sky_colour_top.g);
  rgb[2] = plByteToFloat(manifest_->sky_colour_top.b);
  if (ImGui::ColorEdit3("Top Colour", rgb, ImGuiColorEditFlags_InputRGB)) {
    manifest_->sky_colour_top.r = plFloatToByte(rgb[0]);
    manifest_->sky_colour_top.g = plFloatToByte(rgb[1]);
    manifest_->sky_colour_top.b = plFloatToByte(rgb[2]);
    map_->UpdateSky();
  }

  rgb[0] = plByteToFloat(manifest_->sky_colour_bottom.r);
  rgb[1] = plByteToFloat(manifest_->sky_colour_bottom.g);
  rgb[2] = plByteToFloat(manifest_->sky_colour_bottom.b);
  if (ImGui::ColorEdit3("Bottom Colour", rgb, ImGuiColorEditFlags_InputRGB)) {
    manifest_->sky_colour_bottom.r = plFloatToByte(rgb[0]);
    manifest_->sky_colour_bottom.g = plFloatToByte(rgb[1]);
    manifest_->sky_colour_bottom.b = plFloatToByte(rgb[2]);
    map_->UpdateSky();
  }

  ImGui::Separator();

  ImGui::Text("Lighting Settings");

  if(ImGui::SliderAngle("Sun Pitch", &manifest_->sun_pitch, -180, 180, nullptr)) {
    map_->UpdateLighting();
  }
  if(ImGui::SliderAngle("Sun Yaw", &manifest_->sun_yaw, -180, 180, nullptr)) {
    map_->UpdateLighting();
  }

  rgb[0] = plByteToFloat(manifest_->sun_colour.r);
  rgb[1] = plByteToFloat(manifest_->sun_colour.g);
  rgb[2] = plByteToFloat(manifest_->sun_colour.b);
  if (ImGui::ColorEdit3("Sun Colour", rgb, ImGuiColorEditFlags_InputRGB)) {
    manifest_->sun_colour.r = plFloatToByte(rgb[0]);
    manifest_->sun_colour.g = plFloatToByte(rgb[1]);
    manifest_->sun_colour.b = plFloatToByte(rgb[2]);
    map_->UpdateLighting();
  }

  rgb[0] = plByteToFloat(manifest_->ambient_colour.r);
  rgb[1] = plByteToFloat(manifest_->ambient_colour.g);
  rgb[2] = plByteToFloat(manifest_->ambient_colour.b);
  if (ImGui::ColorEdit3("Ambient Colour", rgb, ImGuiColorEditFlags_InputRGB)) {
    manifest_->ambient_colour.r = plFloatToByte(rgb[0]);
    manifest_->ambient_colour.g = plFloatToByte(rgb[1]);
    manifest_->ambient_colour.b = plFloatToByte(rgb[2]);
    map_->UpdateLighting();
  }

  ImGui::Separator();

  // Fog
  {
    ImGui::Text("Fog Settings");

    rgb[0] = plByteToFloat(manifest_->fog_colour.r);
    rgb[1] = plByteToFloat(manifest_->fog_colour.g);
    rgb[2] = plByteToFloat(manifest_->fog_colour.b);
    if (ImGui::ColorEdit3("Fog Colour", rgb, ImGuiColorEditFlags_InputRGB)) {
      manifest_->fog_colour.r = plFloatToByte(rgb[0]);
      manifest_->fog_colour.g = plFloatToByte(rgb[1]);
      manifest_->fog_colour.b = plFloatToByte(rgb[2]);
      map_->UpdateLighting();
    }

    if (ImGui::SliderFloat("Fog Intensity", &manifest_->fog_intensity, -100.0f, 100.0f, "%.0f")) {
      map_->UpdateLighting();
    }

    if (ImGui::SliderFloat("Fog Distance", &manifest_->fog_distance, 0, 300.0f, "%.0f")) {
      map_->UpdateLighting();
    }
  }

  ImGui::Separator();

  if (ImGui::Button("Save")) {
    std::string dir_name = map_->GetId();
    SaveManifest(std::string(u_get_full_path()) + "/maps/" + dir_name + ".map");
  }

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    RestoreManifest();

    // Close the window
    SetStatus(false);
  }

  ImGui::End();
}

void MapConfigEditor::SaveManifest(const std::string &path) {
  std::ofstream output(path);
  if (!output.is_open()) {
    LogWarn("Failed to write to \"%s\", aborting!n\"\n", filename_buffer);
    return;
  }

  output << "{";

  manifest_->name = name_buffer;
  output << R"("name":")" + manifest_->name + "\",";

  manifest_->name = author_buffer;
  output << R"("author":")" + manifest_->author + "\",";

  output << R"("description":")" + manifest_->description + "\",";
  if (!manifest_->modes.empty()) {
    output << R"("modes":[)";
    for (size_t i = 0; i < manifest_->modes.size(); ++i) {
      output << "\"" + manifest_->modes[i] + "\"";
      if (i != manifest_->modes.size() - 1) {
        output << ",";
      }
    }
    output << "],";
  }

  output << R"("ambientColour":")" +
      std::to_string(manifest_->ambient_colour.r) + " " +
      std::to_string(manifest_->ambient_colour.g) + " " +
      std::to_string(manifest_->ambient_colour.b) + "\",";
  output << R"("skyColourTop":")" +
      std::to_string(manifest_->sky_colour_top.r) + " " +
      std::to_string(manifest_->sky_colour_top.g) + " " +
      std::to_string(manifest_->sky_colour_top.b) + "\",";
  output << R"("skyColourBottom":")" +
      std::to_string(manifest_->sky_colour_bottom.r) + " " +
      std::to_string(manifest_->sky_colour_bottom.g) + " " +
      std::to_string(manifest_->sky_colour_bottom.b) + "\",";
  output << R"("sunColour":")" +
      std::to_string(manifest_->sun_colour.r) + " " +
      std::to_string(manifest_->sun_colour.g) + " " +
      std::to_string(manifest_->sun_colour.b) + "\",";
  output << R"("sunYaw":")" + std::to_string(manifest_->sun_yaw) + "\",";
  output << R"("sunPitch":")" + std::to_string(manifest_->sun_pitch) + "\",";
  output << R"("temperature":")" + manifest_->temperature + "\",";
  output << R"("weather":")" + manifest_->weather + "\",";
  output << R"("time":")" + manifest_->time + "\",";

  // Fog
  output << R"("fogColour":")" +
      std::to_string(manifest_->fog_colour.r) + " " +
      std::to_string(manifest_->fog_colour.g) + " " +
      std::to_string(manifest_->fog_colour.b) + "\",";
  output << R"("fogIntensity":")" + std::to_string(manifest_->fog_intensity) + "\",";
  output << R"("fogDistance":")" + std::to_string(manifest_->fog_distance) + "\"";

  output << "}\n";

  LogInfo("Wrote \"%s\"!\n", path.c_str());
  backup_ = *manifest_;
}

void MapConfigEditor::RestoreManifest() {
  *manifest_ = backup_;

  map_->UpdateSky();
  map_->UpdateLighting();
}

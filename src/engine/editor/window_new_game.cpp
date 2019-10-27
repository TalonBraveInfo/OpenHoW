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
#include "../language.h"

#include "window_new_game.h"

using namespace openhow;

void NewGameWindow::Display() {
  ImGui::SetNextWindowSize(ImVec2(320, 320), ImGuiCond_Once);
  ImGui::Begin("New Game", &status_, ED_DEFAULT_WINDOW_FLAGS);

  // Team Selection
  {
    ImGui::Text("Select Team");
    GameManager::MapManifestMap maps = Engine::Game()->GetMapManifests();
    std::vector<const char*> options;
    for (const auto& map : maps) {
      options.push_back(LanguageManager::GetInstance()->GetTranslation(map.second.name.c_str()));
    }

    static int selected_map = 1;
    if(ImGui::ListBox("Teams", &selected_map, &options[0], options.size(), 10)) {
      snprintf(team_name_, sizeof(team_name_), "%s", std::next(maps.begin(), selected_map)->second.name.c_str());
    }

    ImGui::InputText("Team Name", team_name_, sizeof(team_name_));
  }

  if(ImGui::Button("Configure Team")) {
    // todo
  }

  ImGui::Separator();

  ImGui::Checkbox("Play training mission?", &training_mission_);

  if(ImGui::Button("Start Game")) {
    if(training_mission_) {
      Engine::Game()->LoadMap("camp");
    } else {
      Engine::Game()->LoadMap("estu");
    }

    GameModeDescriptor game_mode_descriptor;
    Engine::Game()->StartMode(game_mode_descriptor);

    SetStatus(false);
  }
  ImGui::SameLine();
  if(ImGui::Button("Cancel")) {
    SetStatus(false);
  }
  ImGui::End();
}

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

#include "NewMapWindow.h"

void NewMapWindow::Display() {
  ImGui::SetNextWindowSize(ImVec2(256, 128), ImGuiCond_Once);
  ImGui::Begin("New Map", &status_, ED_DEFAULT_WINDOW_FLAGS);
  ImGui::InputText("Name", name_buffer_, sizeof(name_buffer_));
  ImGui::InputText("Author", author_buffer_, sizeof(author_buffer_));
  ImGui::Separator();
  if(ImGui::Button("Create")) {}
  ImGui::SameLine();
  if(ImGui::Button("Cancel")) {
    SetStatus(false);
  }
  ImGui::End();
}

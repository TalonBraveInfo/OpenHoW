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
#include "window_actor_tree.h"
#include "../game/actor_manager.h"

using namespace openhow;

ActorTreeWindow::ActorTreeWindow() = default;
ActorTreeWindow::~ActorTreeWindow() = default;

void ActorTreeWindow::Display() {
  ImGui::SetNextWindowSize(ImVec2(310, 512), ImGuiCond_Once);
  ImGui::Begin(dname("Actor Tree"), &status_, ED_DEFAULT_WINDOW_FLAGS);

  const ActorSet actors = ActorManager::GetInstance()->GetActors();
  if(actors.empty()) {
    ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "No actors loaded...");
    ImGui::End();
    return;
  }

  ImGui::End();
}

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

  ImGui::Text("%lu Actors", actors.size());

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));
  size_t num = 0;
  for(auto i : actors) {
    //ImGui::BeginGroup();
    //ImGui::EndGroup();

    if(i->GetParent() != nullptr) {
      // Children get shown under their parents
      continue;
    }

    ImGui::PushID(num);
    ImGui::AlignTextToFramePadding();

    if(ImGui::TreeNode("Actor", "%s (%d %s)", i->GetClassName(), i->GetNumOfChildren(),
        i->GetNumOfChildren() == 1 ? "child" : "children")) {
      DisplayActorProperties(i);
      ImGui::TreePop();
    }

    ImGui::PopID();
  }
  ImGui::PopStyleVar();

  ImGui::End();
}

void ActorTreeWindow::DisplayActorProperties(Actor* actor) {
  static float dummy_members[8] = { 0.0f,0.0f,1.0f,3.1416f,100.0f,999.0f };
  for (int i = 0; i < 8; i++) {
    ImGui::PushID(i); // Use field index as identifier.
    if (i < 2) {
      //ShowDummyObject("Child", 424242);
    } else {
      // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
      ImGui::AlignTextToFramePadding();
      ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "Field_%d", i);
      ImGui::NextColumn();
      ImGui::SetNextItemWidth(-1);
      if (i >= 5)
        ImGui::InputFloat("##value", &dummy_members[i], 1.0f);
      else
        ImGui::DragFloat("##value", &dummy_members[i], 0.01f);
      ImGui::NextColumn();
    }
    ImGui::PopID();
  }
}

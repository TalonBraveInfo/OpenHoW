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

#include "model_viewer.h"

ModelViewer::ModelViewer(const std::string &path) {
  model_ = plLoadModel(path.c_str());
  if(model_ == nullptr) {
    throw std::runtime_error("Failed to load model, \"" + path + "\"!");
  }
  
  model_->model_matrix = plMatrix4Identity();
}

ModelViewer::~ModelViewer() {
  plDestroyModel(model_);
}

void ModelViewer::Display() {
  ImGui::SetNextWindowSize(ImVec2(256, 256), ImGuiCond_Once);
  ImGui::Begin(dname("Model Viewer"), &status_,
      ImGuiWindowFlags_MenuBar |
      ImGuiWindowFlags_HorizontalScrollbar |
      ImGuiWindowFlags_NoSavedSettings);
  
  if(ImGui::BeginMenuBar()) {
    if(ImGui::BeginMenu("View")) {
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  float w = ImGui::GetWindowWidth() - 10;
  ImGui::Image(0, ImVec2(w, w / 2));
  
  ImGui::Text("Path: %s", model_path_.c_str());
  ImGui::End();
}

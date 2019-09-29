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

#include "base_window.h"

class TextureViewer : public BaseWindow {
 public:
  explicit TextureViewer(const std::string &path) {
    texture_ = openhow::Engine::ResourceManagerInstace()->LoadTexture(path, PL_TEXTURE_FILTER_LINEAR);
    if (texture_ == nullptr) {
      throw std::runtime_error("Failed to load specified texture, \"" + path + "\" (" + plGetError() + ")!");
    }

    texture_path = path;
  }

  ~TextureViewer() override {
    plDestroyTexture(texture_, true);
  }

  void ReloadTexture(PLTextureFilter filter_mode) {
    if (filter_mode == filter_mode_) {
      return;
    }

    plDestroyTexture(texture_, true);
    texture_ = openhow::Engine::ResourceManagerInstace()->LoadTexture(texture_path, filter_mode);
    filter_mode_ = filter_mode;
  }

  void Display() override {
    ImGui::SetNextWindowSize(ImVec2(texture_->w + 64, texture_->h + 128), ImGuiCond_Once);
    ImGui::Begin(dname("Texture Viewer"), &status_,
                 ImGuiWindowFlags_MenuBar |
                     ImGuiWindowFlags_HorizontalScrollbar |
                     ImGuiWindowFlags_NoSavedSettings
    );

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("View")) {
        ImGui::SliderInt("Scale", &scale_, 1, 8);
        if (ImGui::BeginMenu("Filter Mode")) {
          if (ImGui::MenuItem("Linear", nullptr, (filter_mode_ == PL_TEXTURE_FILTER_LINEAR))) {
            ReloadTexture(PL_TEXTURE_FILTER_LINEAR);
          }
          if (ImGui::MenuItem("Nearest", nullptr, (filter_mode_ == PL_TEXTURE_FILTER_NEAREST))) {
            ReloadTexture(PL_TEXTURE_FILTER_NEAREST);
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    ImGui::Image(reinterpret_cast<ImTextureID>(texture_->internal.id), ImVec2(
        texture_->w * scale_, texture_->h * scale_));
    ImGui::Separator();
    ImGui::Text("Path: %s", texture_path.c_str());
    ImGui::Text("%dx%d", texture_->w, texture_->h);
    ImGui::Text("Size: %ukB (%luB)", (unsigned int) plBytesToKilobytes(texture_->size),
                (long unsigned) texture_->size);

    ImGui::End();
  }

 protected:
 private:
  PLTexture *texture_{nullptr};
  std::string texture_path;

  PLTextureFilter filter_mode_{PL_TEXTURE_FILTER_LINEAR};

  int scale_{1};
};

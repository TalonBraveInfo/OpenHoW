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

namespace openhow {
class ResourceManager {
 private:
  ResourceManager();
  ~ResourceManager();

 public:
  PLTexture* GetCachedTexture(const std::string& path);
  PLModel* GetCachedModel(const std::string& path);

  PLTexture* LoadTexture(const std::string& path,
                         PLTextureFilter filter = PL_TEXTURE_FILTER_MIPMAP_NEAREST,
                         bool persist = false);
  PLModel* LoadModel(const std::string& path, bool persist = false);

  PLTexture* GetFallbackTexture();
  PLModel* GetFallbackModel();

  void ClearTextures();
  void ClearModels();
  void Clear();

 private:
  struct TextureHandle {
    PLTexture* texture_ptr{nullptr};
    bool persist{false};
  };
  std::map<std::string, TextureHandle> textures_;

  struct ModelHandle {
    PLModel* model_ptr{nullptr};
    bool persist{false};
  };
  std::map<std::string, ModelHandle> models_;

  PLTexture* fallback_texture_{nullptr};
  PLModel* fallback_model_{nullptr};

  friend class Engine;
};
}

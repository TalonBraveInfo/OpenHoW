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

class TextureAtlas {
 public:
  TextureAtlas();
  ~TextureAtlas();

  void AddImage(const std::string &path);
  void AddImages(const std::vector<std::string> &textures);

  void Finalize();

  size_t GetSize();

  struct Index {
    unsigned int x, y, w, h;
  };
  const Index *GetIndex(const std::string &name);
  void GetIndexCoords(const Index *index, float *x, float *y, float *w, float *h);

  PLTexture *GetTexture() { return texture_; }

 protected:
 private:
  std::map<std::string, Index> indices_;
  std::vector<PLImage *> images_;

  unsigned int num_textures_{0};
  PLTexture *texture_{nullptr};

};

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

#include "display.h"
#include "texture_atlas.h"

TextureAtlas::TextureAtlas(const std::vector<std::string> &textures) {
  std::vector<PLImage*> images;
  for(const auto &path : textures) {
    char full_path[PL_SYSTEM_MAX_PATH];
    snprintf(full_path, sizeof(full_path) - 1, "%s", u_find2(path.c_str(), supported_image_formats, true));
    auto* img = static_cast<PLImage *>(u_alloc(1, sizeof(PLImage), true));
    if(!plLoadImage(full_path, img)) {
      Error("Failed to load image (%s)!\n", plGetError());
    }

    plConvertPixelFormat(img, PL_IMAGEFORMAT_RGBA8);
    images.push_back(img);
  }

  // Sort it so the largest are first and smallest come last
  struct {
    bool operator()(const PLImage* a, const PLImage* b) const {
      return a->height < b->height;
    }
  } ImageSizeSort;
  std::sort(images.begin(), images.end(), ImageSizeSort);

  // Figure out how we'll organise the atlas
  unsigned int w = 512, h = 8;
  unsigned int max_h = 0;
  unsigned int cur_y = 0, cur_x = 0;
  for(const auto& image : images) {
    if(image->height > max_h) {
      max_h = image->height;
    }

    if(cur_x == 0 && image->width > w) {
      w = image->width;
    } else if(cur_x + image->width > w) {
      cur_y += max_h;
      cur_x = max_h = 0;
    }

    while(cur_y + image->height > h) {
      h += image->height;
    }

    indices_.emplace(image->path, Index {
      .x = cur_x,
      .y = cur_y,
      .w = w,
      .h = h,
    });

    cur_x += image->width;
  }

  // Now create the atlas itself
  PLImage* cache = plCreateImage(
      nullptr,
      w, h,
      PL_COLOURFORMAT_RGBA,
      PL_IMAGEFORMAT_RGBA8
      );
  if(cache == nullptr) {
    Error("Failed to generate image cache for texture atlas (%s)!\n", plGetError());
  }

  // todo: generate mipmap levels
  cache->data = (uint8_t**)u_alloc(cache->levels, sizeof(uint8_t *), true);
  cache->data[0] = (uint8_t*)u_alloc(cache->size, sizeof(uint8_t), true);
  for(auto& image : images) {
    const Index* index = &indices_[image->path];
    uint8_t* pos = cache->data[0] + ((index->y * cache->width) + index->w) * 4;
    uint8_t* src = image->data[0];
    for(unsigned int y = 0; y < index->h; ++y) {
      memcpy(pos, src, (index->w * 4));
      src += index->w * 4;
      pos += cache->width * 4;
    }

    plFreeImage(image);
    image = nullptr;
  }

  images.clear();

#if 1
  {
    static unsigned int gen_id = 0;
    if(plCreatePath("./debug/atlas_data/")) {
      char buf[PL_SYSTEM_MAX_PATH];
      snprintf(buf, sizeof(buf) - 1, "./debug/atlas_data/%dx%d_%d.png",
          cache->width, cache->height, gen_id++);
      plWriteImage(cache, buf);
    }
  }
#endif

  if((texture_ = plCreateTexture()) == nullptr) {
    Error("Failed to generate atlas texture (%s)!\n", plGetError());
  }

  texture_->filter = PL_TEXTURE_FILTER_NEAREST;
  if(!plUploadTextureImage(texture_, cache)) {
    Error("Failed to upload texture atlas (%s)!\n", plGetError());
  }

  plFreeImage(cache);
}

TextureAtlas::~TextureAtlas() {
  plDestroyTexture(texture_, true);
}

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

#include "engine.h"
#include "resource_manager.h"

using namespace openhow;

ResourceManager::ResourceManager() = default;
ResourceManager::~ResourceManager() {
  plDestroyTexture(fallback_texture_, true);
}

// TODO: we should be able to query the platform library for this!!
const char* supported_model_formats[] = {"obj", "vtx", "min", nullptr};
const char* supported_image_formats[] = {"png", "tga", "bmp", "tim", nullptr};
//const char *supported_audio_formats[]={"wav", NULL};
//const char *supported_video_formats[]={"bik", NULL};

PLTexture* ResourceManager::GetCachedTexture(const std::string& path) {
  auto idx = textures_.find(path);
  if(idx != textures_.end()) {
    return idx->second.texture_ptr;
  }

  return nullptr;
}

PLModel* ResourceManager::GetCachedModel(const std::string& path) {
  auto idx = models_.find(path);
  if(idx != models_.end()) {
    return idx->second.model_ptr;
  }

  return nullptr;
}

PLTexture* ResourceManager::LoadTexture(const std::string& path, PLTextureFilter filter, bool persist) {
  const char* ext = plGetFileExtension(path.c_str());
  if (plIsEmptyString(ext)) {
    const char* fp = u_find2(path.c_str(), supported_image_formats, false);
    if (fp == nullptr) {
      return GetFallbackTexture();
    }

    PLTexture* texture = GetCachedTexture(fp);
    if(texture != nullptr) {
      return texture;
    }

    texture = plLoadTextureImage(fp, filter);
    if (texture != nullptr) {
      return texture;
    }

    LogWarn("%s, aborting!\n", plGetError());
    return GetFallbackTexture();
  }

  const char* fp = u_find(path.c_str());
  PLTexture* texture = GetCachedTexture(fp);
  if(texture != nullptr) {
    return texture;
  }

  PLImage img;
  if (plLoadImage(fp, &img)) {
    // pixel format of TIM will be changed before uploading
    if (pl_strncasecmp(ext, "tim", 3) == 0) {
      plConvertPixelFormat(&img, PL_IMAGEFORMAT_RGBA8);
    }

    texture = plCreateTexture();
    if (texture != nullptr) {
      texture->filter = filter;
      if (plUploadTextureImage(texture, &img)) {
        return texture;
      }
    }
    plDestroyTexture(texture, true);
  }

  LogWarn("Failed to load texture, \"%s\" (%s)!\n", fp, plGetError());
  plFreeImage(&img);

  return GetFallbackTexture();
}

PLTexture* ResourceManager::GetFallbackTexture() {
  if(fallback_texture_ != nullptr) {
    return fallback_texture_;
  }

  PLColour pbuffer[] = {
      {255, 255, 0, 255}, {0, 255, 255, 255},
      {0, 255, 255, 255}, {255, 255, 0, 255}};
  PLImage* image = plCreateImage((uint8_t*) pbuffer, 2, 2, PL_COLOURFORMAT_RGBA, PL_IMAGEFORMAT_RGBA8);
  if (image != nullptr) {
    fallback_texture_ = plCreateTexture();
    fallback_texture_->flags &= PL_TEXTURE_FLAG_NOMIPS;
    if (!plUploadTextureImage(fallback_texture_, image)) {
      Error("Failed to upload default texture (%s)!\n", plGetError());
    }
    plFreeImage(image);
  } else {
    Error("Failed to generate default texture (%s)!\n", plGetError());
  }

  return fallback_texture_;
}

PLModel* ResourceManager::GetFallbackModel() {
  return nullptr;
}

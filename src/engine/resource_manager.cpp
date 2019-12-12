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
#include "graphics/shaders.h"

using namespace openhow;

/* todo:
 *  allow resources to be cached into collections (e.g. Clear("GameTextures"))
 */

PLModel* LoadObjModel(const char* path); // see loaders/obj.cpp
PLModel* Model_LoadVtxFile(const char* path);
PLModel* Model_LoadMinFile(const char* path);

ResourceManager::ResourceManager() {
  plRegisterModelLoader("obj", LoadObjModel);
  plRegisterModelLoader("vtx", Model_LoadVtxFile);
  plRegisterModelLoader("min", Model_LoadMinFile);

  plRegisterConsoleCommand("ListCachedResources", &ResourceManager::ListCachedResources, "List all cached resources.");
  plRegisterConsoleCommand("ClearModels", &ResourceManager::ClearModelsCommand, "Clears all cached models.");
  plRegisterConsoleCommand("ClearTextures", &ResourceManager::ClearTexturesCommand, "Clears all cached textures.");
}

ResourceManager::~ResourceManager() {
  ClearTextures(true);
  ClearModels(true);

  plDestroyTexture(fallback_texture_, true);
  plDestroyModel(fallback_model_);
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

PLTexture* ResourceManager::LoadTexture(const std::string& path, PLTextureFilter filter, bool persist,
    bool abort_on_fail) {
  const char* ext = plGetFileExtension(path.c_str());
  if (plIsEmptyString(ext)) {
    const char* fp = u_find2(path.c_str(), supported_image_formats, abort_on_fail);
    if (fp == nullptr) {
      return CacheTexture(path, GetFallbackTexture(), persist);
    }

    PLTexture* texture = GetCachedTexture(fp);
    if(texture != nullptr) {
      return texture;
    }

    texture = plLoadTextureImage(fp, filter);
    if (texture != nullptr) {
      return CacheTexture(fp, texture, persist);;
    }

    if(abort_on_fail) {
      Error("Failed to load texture, \"%s\" (%s)!\n", fp, plGetError());
    }

    LogWarn("%s, aborting!\n", plGetError());
    return CacheTexture(fp, GetFallbackTexture(), persist);
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
        return CacheTexture(fp, texture, persist);
      }
    }
    plDestroyTexture(texture, true);
  }

  if(abort_on_fail) {
    Error("Failed to load texture, \"%s\" (%s)!\n", fp, plGetError());
  }

  LogWarn("Failed to load texture, \"%s\" (%s)!\n", fp, plGetError());
  plFreeImage(&img);

  return CacheTexture(fp, GetFallbackTexture(), persist);;
}

PLModel* ResourceManager::LoadModel(const std::string& path, bool persist, bool abort_on_fail) {
  const char* fp = u_find2(path.c_str(), supported_model_formats, abort_on_fail);
  if (fp == nullptr) {
    return CacheModel(path, GetFallbackModel(), persist);
  }

  PLModel* model = GetCachedModel(fp);
  if(model != nullptr) {
    return model;
  }

  model = plLoadModel(fp);
  if (model == nullptr) {
    if (abort_on_fail) {
      Error("Failed to load model, \"%s\" (%s)!\n", fp, plGetError());
    }

    LogWarn("Failed to load model, \"%s\" (%s)!\n", fp, plGetError());
    return CacheModel(fp, GetFallbackModel(), persist);
  }

  return CacheModel(fp, model, persist);
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
  if(fallback_model_ != nullptr) {
    return fallback_model_;
  }

  PLMesh* mesh = plCreateMesh(PL_MESH_LINES, PL_DRAW_DYNAMIC, 0, 6);
  plSetMeshVertexPosition(mesh, 0, PLVector3(0, 20, 0));
  plSetMeshVertexPosition(mesh, 1, PLVector3(0, -20, 0));
  plSetMeshVertexPosition(mesh, 2, PLVector3(20, 0, 0));
  plSetMeshVertexPosition(mesh, 3, PLVector3(-20, 0, 0));
  plSetMeshVertexPosition(mesh, 4, PLVector3(0, 0, 20));
  plSetMeshVertexPosition(mesh, 5, PLVector3(0, 0, -20));
  plSetMeshUniformColour(mesh, PLColour(255, 0, 0, 255));
  plSetMeshShaderProgram(mesh, Shaders_GetProgramByName(SHADER_GenericUntextured));
  plUploadMesh(mesh);

  return (fallback_model_ = plCreateBasicStaticModel(mesh));
}

void ResourceManager::ClearTextures(bool force) {
  if(textures_.empty()) {
    return;
  }

  for(auto &i : textures_) {
    if((i.second.persist && !force) || (fallback_texture_ != nullptr && i.second.texture_ptr == fallback_texture_)) {
      continue;
    }

    plDestroyTexture(i.second.texture_ptr, true);
  }

  if(force) {
    textures_.clear();
  }
}

void ResourceManager::ClearModels(bool force) {
  if(models_.empty()) {
    return;
  }

  for(auto &i : models_) {
    if((i.second.persist && !force) || (fallback_model_ != nullptr && i.second.model_ptr == fallback_model_)) {
      continue;
    }

    plDestroyModel(i.second.model_ptr);
  }

  if(force) {
    models_.clear();
  }
}

void ResourceManager::ListCachedResources(unsigned int argc, char** argv) {
  u_unused(argc);
  u_unused(argv);

  LogInfo("Printing cache...\n");

  for(auto const& i : Engine::Resource()->models_) {
    LogInfo(" model %s / %s : name(%s)\n", i.first.c_str(), i.second.persist ? "true" : "false",
        i.second.model_ptr->name);
  }

  unsigned int tsize = 0;
  for(auto const& i : Engine::Resource()->textures_) {
    LogInfo(" texture %s / %s : name(%s)\n", i.first.c_str(), i.second.persist ? "true" : "false",
            i.second.texture_ptr->name);
    tsize += i.second.texture_ptr->size;
  }
  LogInfo("Texture Memory: %dkb\n", plBytesToKilobytes(tsize));
}

void ResourceManager::ClearTexturesCommand(unsigned int argc, char** argv) {
  u_unused(argc);
  u_unused(argv);

  Engine::Resource()->ClearTextures();
}

void ResourceManager::ClearModelsCommand(unsigned int argc, char** argv) {
  u_unused(argc);
  u_unused(argv);

  Engine::Resource()->ClearModels();
}

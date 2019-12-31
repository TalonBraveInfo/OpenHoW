/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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

#include "sprite.h"
#include "display.h"
#include "shaders.h"

Sprite::Sprite(SpriteType type, PLTexture* texture, PLColour colour, float scale) :
    type_(type),
    colour_(colour),
    scale_(scale) {
  mesh_ = plCreateMeshRectangle(0, 0, texture->w, texture->h, colour_);
  mesh_->texture = texture;

  matrix_.Identity();
}

Sprite::~Sprite() = default;

void Sprite::Tick() {
#if 0
  if (current_animation_ == nullptr) {
    return;
  }

  if (frame_delay_ < g_state.sim_ticks + TICKS_PER_SECOND) {
    current_frame_++;
    if (current_frame_ > (current_animation_->h_frames + current_animation_->v_frames)) {
      current_frame_ = 0;
    }
  }
#endif
}

void Sprite::Draw() {
  if (scale_ <= 0) {
    return;
  }

  Shaders_SetProgramByName("generic_textured_lit");

  PLMatrix4 mrot = plRotateMatrix4(angles_.x, PLVector3(1, 0, 0));
  mrot = mrot * plRotateMatrix4(angles_.y, PLVector3(0, 1, 0));
  mrot = mrot * plRotateMatrix4(angles_.z, PLVector3(0, 0, 1));
  matrix_ = mrot * plTranslateMatrix4(position_);

  //matrix_ = matrix_ * PLVector3(scale_, scale_, scale_);

  plSetNamedShaderUniformMatrix4(NULL, "pl_model", matrix_, false);

  plUploadMesh(mesh_);
  plDrawMesh(mesh_);
}

#if 0
void Sprite::SetAnimation(SpriteAnimation* anim) {
  u_assert(anim != nullptr, "Invalid pointer passed for animation!\n");
  current_frame_ = 0;
  current_animation_ = anim;
}
#endif

void Sprite::SetScale(float scale) {
  scale_ = scale;
}

void Sprite::SetPosition(const PLVector3& position) {
  position_ = position;
}

void Sprite::SetAngles(const PLVector3& angles) {
  angles_ = angles;
}

void Sprite::SetColour(const PLColour& colour) {
  plSetMeshUniformColour(mesh_, colour);
  colour_ = colour;
}

void Sprite::SetTexture(PLTexture* texture) {
  // a lot of this will change once the rc manager is introduced...

  if(texture == mesh_->texture) {
    return;
  }

  if(texture->w != mesh_->texture->w || texture->h != mesh_->texture->h) {
    plDestroyMesh(mesh_);
    mesh_ = plCreateMeshRectangle(0, 0, texture->w, texture->h, colour_);
  }

  mesh_->texture = texture;
}

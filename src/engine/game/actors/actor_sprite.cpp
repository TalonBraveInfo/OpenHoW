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

#include "../../engine.h"
#include "../../graphics/sprite.h"
#include "../../graphics/display.h"

#include "../actor_manager.h"

#include "actor.h"

// ASprite
// Can be used for very basic effects etc.

class ASprite : public Actor {
 public:
  ASprite();
  ~ASprite() override;

  void Tick() override;
  void Draw() override;

  virtual void SetSpriteTexture(const std::string& path);
  virtual void SetSpriteTexture(PLTexture* texture);

 protected:
 private:
  Sprite* sprite_;
};

REGISTER_ACTOR(sprite, ASprite)

ASprite::ASprite() {
  sprite_ = new Sprite(Sprite::TYPE_DEFAULT, Display_GetDefaultTexture());
}

ASprite::~ASprite() {
  delete sprite_;
}

void ASprite::SetSpriteTexture(const std::string& path) {
  // TODO: resource manager aaaahhhh!!!
  PLTexture* texture = Display_LoadTexture(path.c_str(), PL_TEXTURE_FILTER_MIPMAP_LINEAR);
  sprite_->SetTexture(texture);
}

void ASprite::SetSpriteTexture(PLTexture* texture) {
  sprite_->SetTexture(texture);
}

void ASprite::Tick() {
  Actor::Tick();

  angles_.x += 0.5f;
  angles_.y += 0.5f;
  angles_.z += 0.5f;

  sprite_->SetPosition(position_);
  sprite_->SetAngles(angles_);

  // increment animation etc.
  sprite_->Tick();
}

void ASprite::Draw() {
  Actor::Draw();

  sprite_->Draw();
}

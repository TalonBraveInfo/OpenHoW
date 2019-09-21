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

  virtual void SetSprite(const std::string& path);

 protected:
 private:
  std::unique_ptr<Sprite> sprite_;
};

register_actor(sprite, ASprite);

ASprite::ASprite() = default;
ASprite::~ASprite() = default;

void ASprite::SetSprite(const std::string& path) {
  //sprite_ = new Sprite(Sprite::TYPE_DEFAULT, );
  //sprite_->SetPosition(position_);
}

void ASprite::Tick() {
  Actor::Tick();
}

void ASprite::Draw() {
  Actor::Draw();

  if(sprite_ == nullptr) {
    return;
  }

  sprite_->Draw();
}

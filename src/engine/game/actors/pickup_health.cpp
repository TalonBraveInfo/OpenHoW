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
#include "../actor_manager.h"
#include "actor_pig.h"

class AHealthPickup : public AModel {
  ACTOR_IMPLEMENT_SUPER(AModel)

 public:
  AHealthPickup();
  ~AHealthPickup() override;

  void Touch(Actor* other) override;

 protected:
 private:
  unsigned int pickup_quantity_{ 0 };
};

REGISTER_ACTOR(crate2, AHealthPickup)

using namespace openhow;

AHealthPickup::AHealthPickup() : SuperClass() {}
AHealthPickup::~AHealthPickup() = default;

void AHealthPickup::Touch(Actor* other) {
  SuperClass::Touch(other);

  APig* pig = dynamic_cast<APig*>(other);
  if(pig == nullptr) {
    // only pigs should be able to pick these up!
    return;
  }

  pig->AddHealth(pickup_quantity_);

  // may want to introduce networking logic here for actor destruction
  // hence why it's done this way for now
  ActorManager::GetInstance()->DestroyActor(this);
}

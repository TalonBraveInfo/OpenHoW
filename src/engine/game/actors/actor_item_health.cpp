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
#include "actor_item.h"
#include "actor_pig.h"

class AItemHealth : public AItem {
  ACTOR_IMPLEMENT_SUPER(AItem)

 public:
  AItemHealth();
  ~AItemHealth() override;

  void PickUp(Actor* other) override;

 protected:
 private:
};

REGISTER_ACTOR(crate2, AItemHealth)

using namespace openhow;

AItemHealth::AItemHealth() : SuperClass() {}
AItemHealth::~AItemHealth() = default;

void AItemHealth::PickUp(Actor* other) {
  SuperClass::PickUp(other);

  APig* pig = dynamic_cast<APig*>(other);
  if(pig == nullptr) {
    // only pigs should be able to pick these up!
    return;
  }

  pig->AddHealth(item_quantity_);

  // may want to introduce networking logic here for actor destruction
  // hence why it's done this way for now
  ActorManager::GetInstance()->DestroyActor(this);
}

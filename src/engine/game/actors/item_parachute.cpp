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
#include "item_parachute.h"
#include "actor_pig.h"

REGISTER_ACTOR(item_parachute, AParachuteItem)

AParachuteItem::AParachuteItem() : SuperClass() {
  SetModel("weapons/we_para");
  ShowModel(false);
}

AParachuteItem::~AParachuteItem() = default;

void AParachuteItem::Tick() {
  SuperClass::Tick();

  if(!is_deployed_) {
    return;
  }

  //APig* pig = dynamic_cast<APig*>(GetParent());
}

void AParachuteItem::Fire(const PLVector3& pos, const PLVector3& dir) {
  SuperClass::Fire(pos, dir);
}

void AParachuteItem::Deploy() {
  SuperClass::Deploy();
}

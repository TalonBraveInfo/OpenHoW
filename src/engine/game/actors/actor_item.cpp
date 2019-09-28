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
#include "../../graphics/display.h"
#include "../actor_manager.h"

#include "actor_item.h"

using namespace openhow;

struct ItemSpawnIndex {

};

AItem::AItem() : AModel() {}
AItem::~AItem() = default;

void AItem::Deserialize(const ActorSpawn& spawn) {
  AModel::Deserialize(spawn);
}

void AItem::PickUp(const Actor* other) {
  u_assert(other != nullptr);
}

PLTexture* AItem::GetInventoryIcon() {
  return Display_GetDefaultTexture();
}

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
#include "actor_manager.h"
#include "inventory.h"

/**
 * Clear the inventory of items.
 */
void InventoryManager::ClearItems() {
  items_.clear();
}

void InventoryManager::AddInventoryItem(ItemIdentifier identifier, unsigned int quantity) {
#if 0
  u_assert(identifier != InventoryItem::Identifier::INVALID_ID, "Attempted to add a null item to inventory!\n");

  LogDebug("Added %s to inventory\n", item->GetInventoryDescription().c_str());

  items_.emplace(std::pair<std::string, InventoryItem*>(item->GetInventoryDescription(), item));
#endif
}

InventoryItem* InventoryManager::GetItem(ItemIdentifier identifier) {
  return nullptr;
}

/// Inventory Items

PLTexture* InventoryItem::GetInventoryIcon() {
  return nullptr;
}

void InventoryItem::Equipped(Actor* other) {

}

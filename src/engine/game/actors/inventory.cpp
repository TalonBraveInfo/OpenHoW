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
#include "inventory.h"

/**
 * Clear the inventory of items.
 */
void InventoryManager::ClearItems() {
  for (auto& item : items_) {
    ActorManager::GetInstance()->DestroyActor(item.second);
    item.second = nullptr;
  }

  items_.clear();
}

void InventoryManager::AddItem(AItem* item) {
  u_assert(item != nullptr, "Attempted to add a null item to inventory!\n");

  LogDebug("Added %s to inventory\n", item->GetInventoryDescription().c_str());

  items_.emplace(std::pair<std::string, AItem*>(item->GetInventoryDescription(), item));
}

void InventoryManager::RemoveItem(AItem* item) {
  items_.find()
}

bool InventoryManager::HasItem(const AItem* item) {
  for (const auto& item : items_) {

  }

  return false;
}

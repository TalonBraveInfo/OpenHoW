// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Inventory.h"

/**
 * Clear the inventory of items.
 */
void InventoryManager::ClearItems() {
	items_.clear();
}

void InventoryManager::AddInventoryItem( ItemIdentifier identifier, unsigned int quantity ) {
#if 0
	u_assert(identifier != InventoryItem::Identifier::INVALID_ID, "Attempted to add a null item to inventory!\n");

	LogDebug("Added %s to inventory\n", item->GetInventoryDescription().c_str());

	items_.emplace(std::pair<std::string, InventoryItem*>(item->GetInventoryDescription(), item));
#endif
}

InventoryItem *InventoryManager::GetItem( ItemIdentifier identifier ) {
	return nullptr;
}

/// Inventory Items

PLGTexture *InventoryItem::GetInventoryIcon() {
	return nullptr;
}

void InventoryItem::Equipped( Actor *other ) {

}

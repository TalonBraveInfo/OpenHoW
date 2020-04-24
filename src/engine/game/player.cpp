/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

#include "player.h"

Player::Player(PlayerType type) : type_(type) {}
Player::~Player() = default;

void Player::PossessCurrentChild() {
	Actor* child = children_[currentChildIndex];
	if(child == nullptr) {
		LogWarn("Child of player is null!\n");
		return;
	}

	if(!child->Possessed(this)) {
		LogWarn("Failed to possess actor!\n");
		return;
	}

	LogDebug("%s possessed child %d...\n", GetTeam()->name.c_str(), currentChildIndex);
}

void Player::DispossessCurrentChild() {
	Actor* actor_ptr = GetCurrentChild();
	if(actor_ptr == nullptr) {
		return;
	}

	actor_ptr->Dispossessed(this);

	LogDebug("%s depossed child %d...\n", GetTeam()->name.c_str(), currentChildIndex);
}

Actor* Player::GetCurrentChild() {
	if(currentChildIndex >= children_.size()) {
		return nullptr;
	}

	return children_[currentChildIndex];
}

void Player::CycleChildren(bool forward) {
	if ( forward ) {
		currentChildIndex++;

		// Check if we need to wrap it round
		if ( currentChildIndex >= children_.size() ) {
			currentChildIndex = 0;
		}

		return;
	}

	// Else we're decrementing

	currentChildIndex--;

	// Check if we need to wrap it round
	if ( currentChildIndex < 0 ) {
		currentChildIndex = children_.size() - 1;
	}

	LogDebug("%s cycled to child %d...\n", GetTeam()->name.c_str(), currentChildIndex);
}

void Player::AddChild(Actor* actor) {
	u_assert(actor != nullptr, "Attempted to pass a null actor reference to player!\n");
	children_.push_back(actor);

	LogDebug("%s received child %d...\n", GetTeam()->name.c_str(), children_.size());
}

void Player::RemoveChild(Actor* actor) {
	// todo
}

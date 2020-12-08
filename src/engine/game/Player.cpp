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

#include "App.h"
#include "Player.h"

Player::Player(PlayerType type) : myType( type) {}
Player::~Player() = default;

void Player::PossessCurrentChild() {
	Actor* child = myChildActors[myCurrentChildActorIndex];
	if(child == nullptr) {
		Warning( "Child of player is null!\n");
		return;
	}

	if(!child->Possessed(this)) {
		Warning( "Failed to possess actor!\n");
		return;
	}

	DebugMsg( "%s possessed child %d...\n", GetTeam()->name.c_str(), myCurrentChildActorIndex);
}

void Player::DispossessCurrentChild() {
	Actor* actor_ptr = GetCurrentChild();
	if(actor_ptr == nullptr) {
		return;
	}

	actor_ptr->Dispossessed(this);

	DebugMsg( "%s depossed child %d...\n", GetTeam()->name.c_str(), myCurrentChildActorIndex);
}

Actor* Player::GetCurrentChild() {
	if( myCurrentChildActorIndex >= myChildActors.size()) {
		return nullptr;
	}

	return myChildActors[myCurrentChildActorIndex];
}

void Player::CycleChildren(bool forward) {
	if ( forward ) {
		myCurrentChildActorIndex++;

		// Check if we need to wrap it round
		if ( myCurrentChildActorIndex >= myChildActors.size() ) {
			myCurrentChildActorIndex = 0;
		}

		return;
	}

	// Else we're decrementing

	myCurrentChildActorIndex--;

	// Check if we need to wrap it round
	if ( myCurrentChildActorIndex < 0 ) {
		myCurrentChildActorIndex = myChildActors.size() - 1;
	}

	DebugMsg( "%s cycled to child %d...\n", GetTeam()->name.c_str(), myCurrentChildActorIndex);
}

void Player::AddChild(Actor* actor) {
	u_assert(actor != nullptr, "Attempted to pass a null actor reference to player!\n");
	myChildActors.push_back( actor);

	DebugMsg( "%s received child %d...\n", GetTeam()->name.c_str(), myChildActors.size());
}

void Player::RemoveChild(Actor* actor) {
	// todo
}

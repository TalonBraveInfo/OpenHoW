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

#include "../engine.h"

#include "player.h"

Player::Player(PlayerType type) : type_(type) {}
Player::~Player() = default;

void Player::PossessChild(unsigned int index) {
  if(index >= children_.size()) {
    LogWarn("Failed to possess actor, child index is out of range (%d vs %d)!\n", index, children_.size());
    return;
  }

  Actor* child = children_[index];
  if(child == nullptr) {
    LogWarn("Child of player is null!\n");
    return;
  }

  if(!child->Possessed(this)) {
    LogWarn("Failed to possess actor!\n");
    return;
  }

  current_child_ = index;
}

void Player::DepossessChild() {
  Actor* actor_ptr = GetCurrentChild();
  if(actor_ptr == nullptr) {
    return;
  }

  actor_ptr->Depossessed(this);
}

Actor* Player::GetCurrentChild() {
  if(current_child_ >= children_.size()) {
    return nullptr;
  }

  return children_[current_child_];
}

void Player::CycleChildren(bool forward) {
  if(forward) {
    if (current_child_ >= children_.size()) {
      current_child_ = 0;
    } else {
      current_child_++;
    }
  } else {
    if(current_child_ == 0) {
      current_child_ = children_.size() - 1;
    } else {
      current_child_--;
    }
  }

  PossessChild(current_child_);
}

void Player::AddChild(Actor* actor) {
  u_assert(actor != nullptr, "Attempted to pass a null actor reference to player!\n");
  children_.push_back(actor);
}

void Player::RemoveChild(Actor* actor) {
  // todo
}

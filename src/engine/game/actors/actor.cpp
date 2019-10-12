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
#include "../../input.h"
#include "../../Map.h"

#include "actor.h"

using namespace openhow;

Actor::Actor() :
  INIT_PROPERTY(input_forward, PROP_PUSH, 0.00),
  INIT_PROPERTY(input_yaw,     PROP_PUSH, 0.00),
  INIT_PROPERTY(input_pitch,   PROP_PUSH, 0.00) {}
Actor::~Actor() = default;

void Actor::SetAngles(PLVector3 angles) {
  VecAngleClamp(&angles);
  old_angles_ = angles_;
  angles_ = angles;
}

void Actor::SetPosition(PLVector3 position) {
  old_position_ = position_;
  position_ = position;
}

void Actor::Deserialize(const ActorSpawn& spawn){
  SetPosition(spawn.position);
  SetAngles(spawn.angles);
}

void Actor::AddHealth(int16_t health) {
  if(health <= 0) {
    return;
  }

  health_ += health;
}

bool Actor::Possessed(const Player* player) {
  return true;
}

void Actor::Depossessed(const Player* player) {}

/**
 * Drop the actor to the ground based on it's bounding box size.
 */
void Actor::DropToFloor() {
  Map* map = Engine::GameManagerInstance()->GetCurrentMap();
  float height = map->GetTerrain()->GetHeight(PLVector2(position_.x, position_.z));
  position_.y = height + bounds_.y;
}

void Actor::HandleInput() {
  IGameMode* mode = Engine::GameManagerInstance()->GetMode();
  if (mode == nullptr) {
    return;
  }

  Player* player = mode->GetCurrentPlayer();
  if (player == nullptr) {
    return;
  }

  PLVector2 cl = Input_GetJoystickState(player->GetControllerSlot(), INPUT_JOYSTICK_LEFT);
  PLVector2 cr = Input_GetJoystickState(player->GetControllerSlot(), INPUT_JOYSTICK_RIGHT);

  if (Input_GetActionState(player->GetControllerSlot(), ACTION_MOVE_FORWARD)) {
    input_forward = 1.0f;
  } else if (Input_GetActionState(player->GetControllerSlot(), ACTION_MOVE_BACKWARD)) {
    input_forward = -1.0f;
  } else {
    input_forward = -cl.y / 327.0f;
  }

  if (Input_GetActionState(player->GetControllerSlot(), ACTION_TURN_LEFT)) {
    input_yaw = -1.0f;
  } else if (Input_GetActionState(player->GetControllerSlot(), ACTION_TURN_RIGHT)) {
    input_yaw = 1.0f;
  } else {
    input_yaw = cr.x / 327.0f;
  }

  if (Input_GetActionState(player->GetControllerSlot(), ACTION_AIM_UP)) {
    input_pitch = 1.0f;
  } else if (Input_GetActionState(player->GetControllerSlot(), ACTION_AIM_DOWN)) {
    input_pitch = -1.0f;
  } else {
    input_pitch = -cr.y / 327.0f;
  }
}

/**
 * Called when one actor collides with another.
 * @param other The touchee.
 */
void Actor::Touch(Actor* other) {
  LogDebug("actor (%s) touched actor (%s)\n",
           plPrintVector3(&position_, pl_int_var),
           plPrintVector3(&other->position_, pl_int_var));
}

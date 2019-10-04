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

#include <PL/platform_graphics_camera.h>

#include "../../engine.h"
#include "../../input.h"
#include "../../Map.h"
#include "../actor_manager.h"
#include "actor_pig.h"

REGISTER_ACTOR(ac_me, APig)    // Ace
REGISTER_ACTOR(gr_me, APig)    // Grunt
REGISTER_ACTOR(hv_me, APig)
REGISTER_ACTOR(le_me, APig)    // Legend
REGISTER_ACTOR(me_me, APig)    // Medic
REGISTER_ACTOR(sa_me, APig)    // Saboteur
REGISTER_ACTOR(sb_me, APig)    // Commando
REGISTER_ACTOR(sn_me, APig)    // Sniper
REGISTER_ACTOR(sp_me, APig)    // Spy

using namespace openhow;

void InventoryManager::Clear() {
  for(auto item : items_) {
    ActorManager::GetInstance()->DestroyActor(item);
    item = nullptr;
  }

  items_.clear();
}

void InventoryManager::AddItem(AItem *item) {
  items_.push_back(item);
}

APig::APig():
  SuperClass(),
  INIT_PROPERTY(input_forward, PROP_PUSH, 0.00),
  INIT_PROPERTY(input_yaw,     PROP_PUSH, 0.00),
  INIT_PROPERTY(input_pitch,   PROP_PUSH, 0.00) {}

APig::~APig() = default;

void APig::HandleInput() {
  IGameMode* mode = Engine::GameManagerInstance()->GetMode();
  if (mode == nullptr) {
    return;
  }

  Player* player = mode->GetCurrentPlayer();
  if (player == nullptr) {
    return;
  }

  PLVector2 cl = Input_GetJoystickState(player->input_slot, INPUT_JOYSTICK_LEFT);
  PLVector2 cr = Input_GetJoystickState(player->input_slot, INPUT_JOYSTICK_RIGHT);

  if (Input_GetActionState(player->input_slot, ACTION_MOVE_FORWARD)) {
    input_forward = 1.0f;
  } else if (Input_GetActionState(player->input_slot, ACTION_MOVE_BACKWARD)) {
    input_forward = -1.0f;
  } else {
    input_forward = -cl.y / 327.0f;
  }

  if (Input_GetActionState(player->input_slot, ACTION_TURN_LEFT)) {
    input_yaw = -1.0f;
  } else if (Input_GetActionState(player->input_slot, ACTION_TURN_RIGHT)) {
    input_yaw = 1.0f;
  } else {
    input_yaw = cr.x / 327.0f;
  }

  if (Input_GetActionState(player->input_slot, ACTION_AIM_UP)) {
    input_pitch = 1.0f;
  } else if (Input_GetActionState(player->input_slot, ACTION_AIM_DOWN)) {
    input_pitch = -1.0f;
  } else {
    input_pitch = -cr.y / 327.0f;
  }
}

void APig::Tick() {
  SuperClass::Tick();

  Camera* camera = Engine::GameManagerInstance()->GetCamera();
  position_.x += input_forward * 100.0f * camera->GetForward().x;
  position_.y += input_forward * 100.0f * camera->GetForward().y;
  position_.z += input_forward * 100.0f * camera->GetForward().z;

  angles_.x += input_pitch * 2.0f;
  angles_.y += input_yaw * 2.0f;

  // Clamp height based on current tile pos
  Map* map = Engine::GameManagerInstance()->GetCurrentMap();
  float height = map->GetTerrain()->GetHeight(PLVector2(position_.x, position_.z));
  if ((position_.y - 32.f) < height) {
    position_.y = height + 32.f;
  }

#define MAX_PITCH 89.f
  if (angles_.x < -MAX_PITCH) angles_.x = -MAX_PITCH;
  if (angles_.x > MAX_PITCH) angles_.x = MAX_PITCH;

  VecAngleClamp(&angles_);
}

void APig::SetClass(int pclass) {
}

void APig::Deserialize(const ActorSpawn& spawn) {
  SuperClass::Deserialize(spawn);

  // ensure pig is spawned up in the air for deployment
  Map* map = Engine::GameManagerInstance()->GetCurrentMap();
  SetPosition({position_.x, map->GetTerrain()->GetMaxHeight(), position_.z});

#if 0
  std::string model_path;
  switch (pclass_) {
    default:
      LogWarn("Unknown pig class, \"%d\", destroying!\n");
      ActorManager::GetInstance()->DestroyActor(this);
      break;

    case CLASS_ACE:
      model_path = "pigs/ac_hi";
      break;
    case CLASS_LEGEND:
      model_path = "pigs/le_hi";
      break;
    case CLASS_MEDIC:
      model_path = "pigs/me_hi";
      break;
    case CLASS_COMMANDO:
      model_path = "pigs/"
      break;
    case CLASS_SPY:break;
    case CLASS_SNIPER:break;
    case CLASS_SABOTEUR:break;
    case CLASS_HEAVY:break;
    case CLASS_GRUNT:break;
  }

  if (!model_path.empty()) {
    SetModel(model_path);
  }
#endif
}

bool APig::Possessed(const Player* player) {
  // TODO
  return false;
}

bool APig::Depossessed(const Player* player) {
  // TODO
  return false;
}

void APig::Killed() {
  // TODO

  inventory_manager_.Clear();
}

/**
 * Add an item into the pig's inventory
 * @param item
 */
void APig::AddInventory(AItem *item) {
  inventory_manager_.AddItem(item);

  // TODO
  //  Display message to user? Or should inventory manager do this...
}

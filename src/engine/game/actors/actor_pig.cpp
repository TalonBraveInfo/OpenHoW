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

register_actor(ac_me, APig);    // Ace
register_actor(le_me, APig);    // Legend
register_actor(me_me, APig);    // Medic
register_actor(sb_me, APig);    // Commando
register_actor(sp_me, APig);    // Spy
register_actor(sn_me, APig);    // Sniper
register_actor(sa_me, APig);    // Saboteur
register_actor(gr_me, APig);    // Grunt

using namespace openhow;

APig::APig(const ActorSpawn& spawn) : AAnimatedModel(spawn) {}

APig::APig() : AAnimatedModel() {}
APig::~APig() = default;

void APig::HandleInput() {
  IGameMode* mode = openhow::engine->GetGameManager()->GetMode();
  if(mode == nullptr) {
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
  position_.x += input_forward * 100.0f * g_state.camera->forward.x;
  position_.y += input_forward * 100.0f * g_state.camera->forward.y;
  position_.z += input_forward * 100.0f * g_state.camera->forward.z;

  angles_.x += input_pitch * 2.0f;
  angles_.y += input_yaw * 2.0f;

  // Clamp height based on current tile pos
  Map* map = engine->GetGameManager()->GetCurrentMap();
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

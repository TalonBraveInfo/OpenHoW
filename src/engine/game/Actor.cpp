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

#include "../engine.h"
#include "../input.h"
#include "../Map.h"

#include "Actor.h"
#include "GameManager.h"

Actor::Actor() = default;
Actor::~Actor() = default;

Actor::Actor(const std::string &name) {
    spawn_name = name;
}

void Actor::HandleInput() {
    Player* player = GameManager::GetInstance()->GetCurrentPlayer();
    if(player == nullptr) {
        return;
    }

    PLVector2 cl = Input_GetJoystickState(player->input_slot, INPUT_JOYSTICK_LEFT);
    PLVector2 cr = Input_GetJoystickState(player->input_slot, INPUT_JOYSTICK_RIGHT);

    float forward_speed = -cl.y;
    if(Input_GetActionState(player->input_slot, ACTION_MOVE_FORWARD)) {
        forward_speed = 100.0f;
    } else if(Input_GetActionState(player->input_slot, ACTION_MOVE_BACKWARD)) {
        forward_speed = -100.0f;
    }

    position_.x += forward_speed * g_state.camera->forward.x;
    position_.y += forward_speed * g_state.camera->forward.y;
    position_.z += forward_speed * g_state.camera->forward.z;

    if(Input_GetActionState(player->input_slot, ACTION_SELECT)) {
        if (Input_GetActionState(player->input_slot, ACTION_TURN_LEFT)) {
            position_.x += 100.f;
        } else if (Input_GetActionState(player->input_slot, ACTION_TURN_RIGHT)) {
            position_.x -= 100.f;
        }
    } else {
        if (Input_GetActionState(player->input_slot, ACTION_TURN_LEFT)) {
            angles_.y -= 2.f;
        } else if (Input_GetActionState(player->input_slot, ACTION_TURN_RIGHT)) {
            angles_.y += 2.f;
        } else {
            angles_.y += cr.x / 50.f;
            angles_.x -= cr.y / 50.f;
        }
    }

    if( Input_GetActionState(player->input_slot, ACTION_JUMP) ||
        Input_GetActionState(player->input_slot, ACTION_AIM_UP)) {
        position_.y += 100.f;
    } else if(Input_GetActionState(player->input_slot, ACTION_AIM_DOWN)) {
        position_.y -= 100.f;
    }

    // Clamp height based on current tile pos
    Map* map = GameManager::GetInstance()->GetCurrentMap();
    float height = map->GetHeight(PLVector2(position_.x, position_.z));
    if((position_.y - 32.f) < height) {
        position_.y = height + 32.f;
    }

    if(angles_.x < -70) angles_.x = -70;
    if(angles_.x > 70) angles_.x = 70;

    VecAngleClamp(&angles_);
}

void Actor::SetAngles(PLVector3 angles) {
    VecAngleClamp(&angles);
    angles_ = angles;
    /* todo: limit angles... */
}

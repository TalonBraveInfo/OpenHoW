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

Actor::~Actor() = default;

Actor::Actor():
    position_(*this, "position", ActorProperty::DEFAULT, PLVector3(0, 0, 0)),
    angles_(*this, "angles", ActorProperty::DEFAULT, PLVector3(0, 0, 0)),
    move_forward(*this, "move_forward", ActorProperty::INPUT, false),
    move_backward(*this, "move_backward", ActorProperty::INPUT, false),
    move_up(*this, "move_up", ActorProperty::INPUT, false),
    move_down(*this, "move_down", ActorProperty::INPUT, false),
    turn_left(*this, "turn_left", ActorProperty::INPUT, false),
    turn_right(*this, "turn_right", ActorProperty::INPUT, false) {}

Actor::Actor(const std::string &name):
    position_(*this, "position", ActorProperty::DEFAULT, PLVector3(0, 0, 0)),
    angles_(*this, "angles", ActorProperty::DEFAULT, PLVector3(0, 0, 0)),
    move_forward(*this, "move_forward", ActorProperty::INPUT, false),
    move_backward(*this, "move_backward", ActorProperty::INPUT, false),
    move_up(*this, "move_up", ActorProperty::INPUT, false),
    move_down(*this, "move_down", ActorProperty::INPUT, false),
    turn_left(*this, "turn_left", ActorProperty::INPUT, false),
    turn_right(*this, "turn_right", ActorProperty::INPUT, false)
{
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

    move_forward = forward_speed > 0.0f;
    move_backward = forward_speed < 0.0f;
    
    turn_left = false;
    turn_right = false;

    if(Input_GetActionState(player->input_slot, ACTION_SELECT)) {
//         if (Input_GetActionState(player->input_slot, ACTION_TURN_LEFT)) {
//             position_.x += 100.f;
//         } else if (Input_GetActionState(player->input_slot, ACTION_TURN_RIGHT)) {
//             position_.x -= 100.f;
//         }
    } else {
        if (Input_GetActionState(player->input_slot, ACTION_TURN_LEFT)) {
//             angles_.y -= 2.f;
            turn_left = true;
        } else if (Input_GetActionState(player->input_slot, ACTION_TURN_RIGHT)) {
//             angles_.y += 2.f;
            turn_right = true;
        } else {
//             angles_.y += cr.x / 50.f;
//             angles_.x -= cr.y / 50.f;
        }
    }

    move_up = false;
    move_down = false;

    if( Input_GetActionState(player->input_slot, ACTION_JUMP) ||
        Input_GetActionState(player->input_slot, ACTION_AIM_UP)) {
        move_up = true;
    } else if(Input_GetActionState(player->input_slot, ACTION_AIM_DOWN)) {
        move_down = true;
    }
}

void Actor::Tick()
{
    PLVector3 forward = plVector3Normalize(PLVector3(
            cosf(plToRadians(angles_.y)) * cosf(plToRadians(angles_.x)),
            sinf(plToRadians(angles_.x)),
            sinf(plToRadians(angles_.y)) * cosf(plToRadians(angles_.x))
    ));

    if(move_forward)
    {
        position_.x += 100.0f * forward.x;
        position_.y += 100.0f * forward.y;
        position_.z += 100.0f * forward.z;
    }
    else if(move_backward)
    {
        position_.x -= 100.0f * forward.x;
        position_.y -= 100.0f * forward.y;
        position_.z -= 100.0f * forward.z;
    }

    if(move_up)
    {
        position_.y += 100.0f;
    }
    else if(move_down)
    {
        position_.y -= 100.0f;
    }

    if(turn_left)
    {
        angles_.y -= 2.0f;
    }
    else if(turn_right)
    {
        angles_.y += 2.0f;
    }

    // Clamp height based on current tile pos
    Map* map = GameManager::GetInstance()->GetCurrentMap();
    float height = map->GetHeight(PLVector2(position_.x, position_.z));
    if((position_.y - 32.f) < height) {
        position_.y = height + 32.f;
    }

    if(angles_.x < -70) angles_.x = -70;
    if(angles_.x > 70) angles_.x = 70;

    PLVector3 a = angles_;
    VecAngleClamp(&a);
    angles_ = a;
}

void Actor::SetAngles(PLVector3 angles) {
    VecAngleClamp(&angles);
    angles_ = angles;
    /* todo: limit angles... */
}

ActorProperty::ActorProperty(Actor &actor, const std::string &name, Flags flags):
	flags(flags),
	name(name),
	actor(actor),
	dirty(false)
{
	auto x = actor.properties_.insert(std::make_pair(name, this));
	assert(x.second);
}

ActorProperty::~ActorProperty()
{
	actor.properties_.erase(name);
}

void ActorProperty::mark_dirty()
{
    if(!dirty)
    {
        dirty_tick = g_state.sim_ticks;
        dirty = true;
    }
}

void ActorProperty::mark_clean()
{
    dirty = false;
}

bool ActorProperty::is_dirty() const
{
    return dirty;
}

unsigned int ActorProperty::dirty_ticks() const
{
    if(dirty)
    {
        assert(g_state.sim_ticks >= dirty_tick);
        return g_state.sim_ticks - dirty_tick;
    }
    else{
        return 0;
    }
}

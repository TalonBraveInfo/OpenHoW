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

#include "actor.h"

Actor::Actor() = default;
Actor::~Actor() = default;

Actor::Actor(const std::string &name) {
    spawn_name = name;
}

void Actor::HandleInput() {}

void Actor::SetAngles(PLVector3 angles) {
    VecAngleClamp(&angles);
    angles_ = angles;
    /* todo: limit angles... */
}

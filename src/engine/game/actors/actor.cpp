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
#include "actor.h"

Actor::Actor() = default;
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

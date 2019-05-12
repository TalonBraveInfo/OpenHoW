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
#include "../model.h"

#include "ActorManager.h"
#include "StaticModelActor.h"

register_actor(DUMMY, StaticModelActor); // todo: needs special logic

// Barbed Wire
register_actor(BARBWIRE, StaticModelActor);
register_actor(BARBWIR2, StaticModelActor);

// Trees
register_actor(TREEP, StaticModelActor);
register_actor(TREEPH1, StaticModelActor);
register_actor(TREEPH2, StaticModelActor);

// Bridges
register_actor(BRIDGE_C, StaticModelActor);
register_actor(BRIDGE_S, StaticModelActor);

// Building Components
register_actor(IRONGATE, StaticModelActor);

StaticModelActor::StaticModelActor() {
    SetModel("decoration/blah");
}

StaticModelActor::~StaticModelActor() = default;

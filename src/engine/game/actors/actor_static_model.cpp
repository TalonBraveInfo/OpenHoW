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
#include "../actor_manager.h"
#include "actor_static_model.h"

register_actor(DUMMY, AStaticModel); // todo: needs special logic
register_actor(SIGN, AStaticModel);
register_actor(CRATE1, AStaticModel);
register_actor(CRATE2, AStaticModel);

// Barbed Wire
register_actor(BARBWIRE, AStaticModel);
register_actor(BARBWIR2, AStaticModel);

// Trees
register_actor(TREEP, AStaticModel);
register_actor(TREEPH, AStaticModel);
register_actor(TREEPH1, AStaticModel);
register_actor(TREEPH2, AStaticModel);

// Bridges
register_actor(BRIDGE_C, AStaticModel);
register_actor(BRID2_C, AStaticModel);
register_actor(BRIDG_C2, AStaticModel);
register_actor(BRIDGE_S, AStaticModel);

// Building Components
register_actor(IRONGATE, AStaticModel);
register_actor(STF03PPP, AStaticModel);
register_actor(STW04_W_, AStaticModel);
register_actor(STW04_D2, AStaticModel);
register_actor(STW04PPP, AStaticModel);
register_actor(STW05_W_, AStaticModel);
register_actor(STW05PPP, AStaticModel);
register_actor(STW06_W_, AStaticModel);
register_actor(STW06PPP, AStaticModel);
register_actor(STW07PWW, AStaticModel);

// Buildings
register_actor(SHELTER, AStaticModel);

AStaticModel::AStaticModel(const ActorSpawn& spawn) : AModel(spawn) {
  SetModel("scenery/" + spawn.class_name);

  if(spawn.class_name == "brid2_s") {
    angles_.z = plDegreesToRadians(-45.f);
  }
}

AStaticModel::AStaticModel() : AModel() {}
AStaticModel::~AStaticModel() = default;


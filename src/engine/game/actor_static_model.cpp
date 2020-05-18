/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

#include "actor_manager.h"
#include "actor_static_model.h"

REGISTER_ACTOR_BASIC( AStaticModel )

AStaticModel::AStaticModel() : SuperClass() {}
AStaticModel::~AStaticModel() = default;

void AStaticModel::Deserialize( const ActorSpawn &spawn ) {
	SuperClass::Deserialize( spawn );

	SetModel( "scenery/" + spawn.class_name );

	// Some models are horrible special cases
	if ( spawn.class_name == "brid2_s" ) {
		SetAngles( PLVector3( angles_.GetValue().x, plDegreesToRadians( -45.f ), angles_.GetValue().z ) );
	}
}

void AStaticModel::Draw() {
	SuperClass::Draw();
}

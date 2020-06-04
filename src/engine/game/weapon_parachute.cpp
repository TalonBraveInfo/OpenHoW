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
#include "weapon_parachute.h"

// Parachute, not really a weapon but hey ho!

REGISTER_ACTOR( weapon_parachute, AParachuteWeapon )

AParachuteWeapon::AParachuteWeapon() : SuperClass() {
	SetModel( "weapons/we_para.vtx" );
	ShowModel( false );
}

AParachuteWeapon::~AParachuteWeapon() = default;

void AParachuteWeapon::Tick() {
	SuperClass::Tick();

	if ( !isWeaponDeployed ) {
		return;
	}

	/* todo: make parachute noises... */
}

void AParachuteWeapon::Fire( const PLVector3 &pos, const PLVector3 &dir ) {
	SuperClass::Fire( pos, dir );
}

void AParachuteWeapon::Deploy() {
	SuperClass::Deploy();
}

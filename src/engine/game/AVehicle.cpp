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

#include "App.h"
#include "AVehicle.h"

using namespace ohw;

AVehicle::AVehicle() : SuperClass() {}

AVehicle::~AVehicle() {
	delete audioSource;

	myOccupants.clear();
}

void AVehicle::Tick() {
	SuperClass::Tick();

	audioSource->SetPosition( position_ );
}

bool AVehicle::EnterVehicle( Actor *occupant ) {
	// TODO: pass by reference, not by pointer...
	if ( GetMaxOccupants() == 0 || myOccupants.size() > GetMaxOccupants() ) {
		return false;
	}

	myOccupants.push_back( occupant );

	return true;
}

void AVehicle::ExitVehicle( Actor *occupant ) {
	// TODO: pass by reference, not by pointer...
}

bool AVehicle::IsOccupied() const {
	return ( myOccupants.size() > 0 );
}

Actor *AVehicle::GetOccupant( unsigned int slot ) {
	if ( slot > myOccupants.size() || slot >= GetMaxOccupants() ) {
		Warning( "Invalid occupant slot, \"%d\"!\n", slot );
		return nullptr;
	}

	return myOccupants[ slot ];
}

void AVehicle::Deserialize( const ActorSpawn &spawn ) {
	SuperClass::Deserialize( spawn );

	audioSource = GetApp()->audioManager->CreateSource();
	audioSource->SetPosition( position_ );
	audioSource->SetLooping( true );

	const AudioSample *audioSample = GetApp()->audioManager->CacheSample( GetEngineIdleSound() );
	if ( audioSample != nullptr ) {
		audioSource->SetSample( audioSample );
		audioSource->StartPlaying();
	}
}

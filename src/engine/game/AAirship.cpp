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
#include "ActorManager.h"
#include "AAirship.h"
#include "Map.h"

REGISTER_ACTOR_BASIC( AAirship )

using namespace ohw;

#define AIRSHIP_SPEED       16.0f
#define AIRSHIP_TURN_LIMIT  32

AAirship::AAirship() : SuperClass() {}

AAirship::~AAirship() {
	delete ambientSource;
}

void AAirship::Tick() {
	SuperClass::Tick();

	Map *map = ohw::GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	PLVector3 position = GetPosition();
	PLVector3 airshipToDestination = myDestination - position;
	float distance = airshipToDestination.Length();
	if ( distance < myDestinationTolerance ) {
		// Set a destination for us to start heading towards.
		PLVector2 point = map->GetRandomPointInPlayArea();
		myDestination = PLVector3( point.x, GetHeight(), point.y );
		DebugMsg( "Set destination at %s\n", plPrintVector3( &myDestination, pl_int_var ) );

		myDestinationTolerance = 5.0f;
		myTurnFrames = 0;
		return;
	}

	PLVector3 angles = GetAngles();
	float curAngle = std::atan2( myForward.z, myForward.x );
	float targetAngle = std::atan2( airshipToDestination.z, airshipToDestination.x );
	float deltaAngle = targetAngle - curAngle;
	if ( std::abs( deltaAngle ) > ( PL_PI / 8.0f ) ) {
		angles.y = myTurnSpeed;
		angles.z++;
		angles.z = std::min( angles.z, 45.0f );

		// Increase tolerance after X frames to avoid infinite circling
		if ( ++myTurnFrames > AIRSHIP_TURN_LIMIT ) {
			myDestinationTolerance *= 2.0f;
			myTurnFrames = 0;
		}
	} else {
		angles.y = 0;
		angles.z--;
		angles.z = std::max( angles.z, -45.0f );
	}

	SetAngles( angles );

	position += myForward * AIRSHIP_SPEED;
	SetPosition( position );

	ambientSource->SetPosition( GetPosition() );
}

void AAirship::Deserialize( const ActorSpawn &spawn ) {
	SuperClass::Deserialize( spawn );

	ambientSource = GetApp()->audioManager->CreateSource(
			"audio/en_bip.wav",
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f },
			true,
			1.0f,
			1.0f,
			true );
	ambientSource->StartPlaying();

	SetModel( "scenery/airship1.vtx" );
	SetAngles( { 180.0f, 0.0f, 0.0f } );

	ShowModel( true );

	Map *map = ohw::GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	// Set a destination for us to start heading towards.
	PLVector2 point = map->GetRandomPointInPlayArea();
	myDestination = PLVector3( point.x, GetHeight(), point.y );
}

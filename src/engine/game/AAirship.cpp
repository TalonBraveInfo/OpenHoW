// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "ActorManager.h"
#include "AAirship.h"
#include "Map.h"

REGISTER_ACTOR_BASIC( AAirship )

using namespace ohw;

#define AIRSHIP_SPEED       16.0f
#define AIRSHIP_TURN_LIMIT  32

AAirship::AAirship() : SuperClass() {}
AAirship::~AAirship() = default;

void AAirship::Tick() {
	SuperClass::Tick();

	Map *map = ohw::GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	hei::Vector3 position = GetPosition();
	hei::Vector3 airshipToDestination = myDestination - position;
	float distance = airshipToDestination.Length();
	if ( distance < myDestinationTolerance ) {
		// Set a destination for us to start heading towards.
		PLVector2 point = map->GetRandomPointInPlayArea();
		myDestination = hei::Vector3( point.x, GetHeight(), point.y );
		DebugMsg( "Set destination at %s\n", PlPrintVector3( &myDestination, PL_VAR_I32 ) );

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

	GetApp()->GetDisplay()->DebugDrawLine( position_, myDestination, PL_COLOUR_RED );
}

void AAirship::Deserialize( const ActorSpawn &spawn ) {
	SuperClass::Deserialize( spawn );

	SetModel( "scenery/airship1.vtx" );
	SetAngles( { 180.0f, 0.0f, 0.0f } );

	ShowModel( true );

	Map *map = ohw::GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	PLVector2 point;
	// Set us up wherever
	point = map->GetRandomPointInPlayArea();
	SetPosition( hei::Vector3( point.x, map->GetTerrain()->GetMaxHeight(), point.y ) );
	// Set a destination for us to start heading towards.
	point = map->GetRandomPointInPlayArea();
	myDestination = hei::Vector3( point.x, GetHeight(), point.y );
}

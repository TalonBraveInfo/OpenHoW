// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "ActorManager.h"
#include "AStaticModel.h"

REGISTER_ACTOR_BASIC( AStaticModel )

AStaticModel::AStaticModel() : SuperClass() {}
AStaticModel::~AStaticModel() = default;

void AStaticModel::Deserialize( const ActorSpawn &spawn ) {
	SuperClass::Deserialize( spawn );

	SetModel( "scenery/" + spawn.className + ".vtx" );

	// Some models are horrible special cases
	if ( spawn.className == "brid2_s" ) {
		SetAngles( hei::Vector3( myAngles.GetValue().x, PlDegreesToRadians( -45.f ), myAngles.GetValue().z ) );
	}
}

void AStaticModel::Draw() {
	SuperClass::Draw();
}

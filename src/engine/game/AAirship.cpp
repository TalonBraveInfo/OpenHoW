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

REGISTER_ACTOR_BASIC( AAirship )

using namespace ohw;

AAirship::AAirship() : SuperClass() {}

AAirship::~AAirship() {
	delete ambientSource;
}

void AAirship::Tick() {
	SuperClass::Tick();

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
}

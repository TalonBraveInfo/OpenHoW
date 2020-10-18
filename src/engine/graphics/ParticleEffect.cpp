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
#include "json_reader.h"
#include "particles.h"
#include "ParticleEffect.h"
#include "ParticleEmitter.h"

ParticleEffect::ParticleEffect( const std::string &effectPath ) {
	try {
		JsonReader jsonBlob( effectPath );

		int version = jsonBlob.GetIntegerProperty( "version", 0, false );
		if ( version != PARTICLE_EFFECT_VERSION ) {
			Warning( "Invalid particle effect version, %d (expected %d) in \"%s\"!\n",
			         version,
			         PARTICLE_EFFECT_VERSION,
			         effectPath.c_str());
		}

		unsigned int numEmitters = jsonBlob.GetArrayLength( "emitters" );
		if ( numEmitters == 0 ) {
			Warning( "No emitters in particle effect \"%s\"!\n", effectPath.c_str());
		}

		jsonBlob.EnterChildNode( "emitters" );
		for ( unsigned int i = 0; i < numEmitters; ++i ) {
			jsonBlob.EnterChildNode( i );

			ParticleEmitter emitter( jsonBlob );
			myEmitters.push_back( emitter );

			jsonBlob.LeaveChildNode();
		}
		jsonBlob.LeaveChildNode();
	} catch( const std::exception &exception ) {

	}
}

ParticleEffect::~ParticleEffect() {

}

void ParticleEffect::Draw() {
	for( auto &i : myEmitters ) {
		i.Draw();
	}
}

void ParticleEffect::Tick() {
	for( auto &i : myEmitters ) {
		i.Tick();
	}
}

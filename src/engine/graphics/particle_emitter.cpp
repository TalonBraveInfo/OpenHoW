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
#include "../script/json_reader.h"

#include "particles.h"
#include "particle_emitter.h"

ParticleEmitter::ParticleEmitter( JsonReader &jsonReader ) {
	myGravity = jsonReader.GetFloatProperty( "gravity" );

	randomLifeSpanFactor = jsonReader.GetFloatProperty( "randomLifeSpawnFactor" );
	lifeSpan = jsonReader.GetFloatProperty( "lifeSpan" );

	// Colour
	randomColourFactor = jsonReader.GetVector4Property( "randomColourFactor" );
	startColour = jsonReader.GetColourProperty( "startColour" );
	endColour = jsonReader.GetColourProperty( "endColour" );

	// Scaling
	randomScaleFactor = jsonReader.GetFloatProperty( "randomScaleFactor" );
	startScale = jsonReader.GetFloatProperty( "startScale" );
	endScale = jsonReader.GetFloatProperty( "endScale" );

	drawDistance = jsonReader.GetFloatProperty( "drawDistance", 10000.0f, true );
	spawnRadius = jsonReader.GetFloatProperty( "spawnRadius" );

	sortParticles = jsonReader.GetBooleanProperty( "sortParticles" );
	collideActors = jsonReader.GetBooleanProperty( "collideActors" );
	collideWorld = jsonReader.GetBooleanProperty( "collideWorld" );

	maxParticles = jsonReader.GetIntegerProperty( "maxParticles" );

	std::string parsedString;

	// Particle type specifies whether or not we're a model, sprite, trail or text
	parsedString = jsonReader.GetStringProperty( "particleType", "sprite", true );
	if ( parsedString == "sprite" ) {
		myParticleType = ParticleType::SPRITE;
	} else if ( parsedString == "model" ) {
		myParticleType = ParticleType::MODEL;
	} else if ( parsedString == "trail" ) {
		myParticleType = ParticleType::TRAIL;
	} else if ( parsedString == "text" ) {
		myParticleType = ParticleType::TEXT;
	} else {
		myParticleType = ParticleType::SPRITE;
		Warning( "Invalid particle type \"%s\" specified, defaulting to \"sprite\"!\n", parsedString.c_str() );
	}

	// And blend type specifies how we should draw ourselves
	parsedString = jsonReader.GetStringProperty( "blendType", "additive", true );
	if ( parsedString == "additive" ) {
		myBlendType = BlendType::ADDITIVE;
	} else if ( parsedString == "subtractive" ) {
		myBlendType = BlendType::SUBTRACTIVE;
	} else if ( parsedString == "difference" ) {
		myBlendType = BlendType::DIFFERENCE;
	} else {
		myBlendType = BlendType::ADDITIVE;
		Warning( "Invalid blend type \"%s\" specified, default to \"additive\"!\n", parsedString.c_str() );
	}

	isActive = jsonReader.GetBooleanProperty( "isActive" );
}

void ParticleEmitter::Tick() {
	for ( auto &i : particles ) {
		i.Tick();
	}
}

void ParticleEmitter::Draw() {
	// TODO: figure out how we're going to batch these depending on type...
}

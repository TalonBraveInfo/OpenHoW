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

#include "particles.h"

Particle::Particle( PLVector3 position, PLVector3 velocity, PLColour startColour, float lifeSpan ) :
	myColour( startColour ),
	myVelocity( velocity ),
	myPosition( position ),
	myLifeSpan( lifeSpan )
{}

// SpriteParticle

SpriteParticle::SpriteParticle( const std::string &texturePath, PLColour startColour, PLVector3 position, PLVector3 velocity, float lifeSpan ) :
	Particle( position, velocity, startColour, lifeSpan ),
	Sprite( SpriteType::TYPE_DEFAULT, texturePath ) {}

SpriteParticle::~SpriteParticle() {}

// ModelParticle

ModelParticle::ModelParticle( const std::string &modelPath, PLVector3 position, PLVector3 velocity, PLColour startColour, float lifeSpan ) : Particle( position, velocity, startColour, lifeSpan ) {
	modelPtr = ohw::Engine::Resource()->LoadModel( modelPath );
}

ModelParticle::~ModelParticle() {}

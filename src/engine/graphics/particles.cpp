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

SpriteParticle::SpriteParticle( PLTexture *texture, PLColour startColour, PLVector3 position, PLVector3 velocity, float lifeSpan ) :
	Particle( position, velocity, startColour, lifeSpan ),
	Sprite( SpriteType::TYPE_DEFAULT, texture ) {

}

SpriteParticle::~SpriteParticle() {}

// ModelParticle

ModelParticle::ModelParticle( PLVector3 position, PLVector3 velocity, PLColour startColour, float lifeSpan, const std::string &modelPath ) : Particle( position, velocity, startColour, lifeSpan ) {
	myModelPtr = openhow::Engine::Resource()->GetCachedModel( modelPath );
}

//ModelParticle::ModelParticle( PLModel *modelPtr, PLVector3 position, PLVector3 velocity, PLColour startColour, float lifeSpan ) : Particle( position, velocity, startColour, lifeSpan ) {
//	myModelPtr = modelPtr;
//}

ModelParticle::~ModelParticle() {
	// TODO: remove reference to model instance
}

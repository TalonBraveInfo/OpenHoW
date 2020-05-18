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

#pragma once

#include "sprite.h"

// And this is the individual particle
class Particle {
public:
	Particle( PLVector3 position, PLVector3 velocity, PLColour startColour, float lifeSpan );

	void Tick() {}

protected:
private:
	PLColour    myColour;
	PLVector3   myVelocity;
	PLVector3   myScale{ 0.0f, 0.0f, 0.0f };
	PLVector3   myPosition;

	float myLifeSpan{ 0.0f };
};

class SpriteParticle : public Particle, Sprite {
	SpriteParticle( PLTexture *texture, PLColour startColour, PLVector3 position, PLVector3 velocity, float lifeSpan );
	~SpriteParticle();

private:
};

class ModelParticle : public Particle {
	ModelParticle( PLVector3 position,
	               PLVector3 velocity,
	               PLColour start_colour,
	               float life_span,
	               const std::string &modelPath );
	ModelParticle( PLModel *modelPtr, PLVector3 position, PLVector3 velocity, float lifeSpan );
	~ModelParticle();

private:
	PLModel *myModelPtr{ nullptr };
};

// TODO
class TrailParticle : public Particle {};
class TextParticle : public Particle {};

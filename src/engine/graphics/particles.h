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

class Particle {
 public:
	Particle( PLVector3 position, PLVector3 velocity, float lifeSpan );

	void Tick() {}

 protected:
 private:
	PLVector3 velocity;
 	float     lifeSpan{ 0.0f };
};

class SpriteParticle : public Particle, Sprite {};
class ModelParticle : public Particle {};
class TrailParticle : public Particle {};
class TextParticle : public Particle {};

class ParticleEmitter : public PropertyOwner {
public:
	ParticleEmitter( PLVector3 position ) {}
	~ParticleEmitter() {}

	void Tick();
	void Draw();

protected:
private:
	std::vector< Particle > particles;

	float gravity{ 0.0f };

	float randomLifeSpanFactor{ 0.0f };
	float lifeSpan{ 1.0f };

	PLVector4 randomColourFactor;
	PLColour startColour;
	PLColour endColour;

	float randomScaleFactor{ 0.0f };
	float startScale{ 0.0f };
	float endScale{ 1.0f };

	float drawDistance{ 10000.0f };

	float spawnRadius{ 0.0f };

	bool sortParticles{ false };

	// todo; consider more typical collision mask?
	bool collideActors{ false };
	bool collideWorld{ false };

	unsigned int maxParticles{ 0 };

	enum class ParticleType {
		SPRITE,
		MODEL,
		TRAIL,
		TEXT,
	};
	enum class BlendType {
		NONE,
		ADDITIVE,
		SUBTRACTIVE,
		DIFFERENCE
	};

	PLVector3 position;
};

class ParticleEffect {
 public:
	ParticleEffect() {}
	~ParticleEffect() {}

	void Draw();
	void Tick();

 protected:
 private:
	std::string name;

 	unsigned int version{ 0 };

 	PLVector3 position;

	std::map< std::string, ParticleEmitter > emitters;
};

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

//#include "Property.h"

namespace ohw {
	class Particle;

// This creates the particles
	class ParticleEmitter : public PropertyOwner {
	public:
		explicit ParticleEmitter( JsonReader &jsonReader );
		~ParticleEmitter() {}

		void Tick();
		void Draw();

		void SetGravity( float gravity ) { myGravity = gravity; }
		float GetGravity() { return myGravity; }

	protected:
	private:
		std::vector <Particle> particles;

		float myGravity{ 0.0f };

		float randomLifeSpanFactor{ 0.0f };
		float lifeSpan{ 1.0f };

		PLVector4 randomColourFactor{ 0.0f, 0.0f, 0.0f, 0.0f };
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

		ParticleType myParticleType{ ParticleType::SPRITE };
		BlendType myBlendType{ BlendType::ADDITIVE };

		bool isActive{ false };

		PLVector3 position;
	};
}

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
#include "graphics/ParticleEffect.h"
#include "graphics/ParticleEmitter.h"
#include "graphics/particles.h"
#include "ActorManager.h"

class AParticleEffect : public Actor {
	IMPLEMENT_ACTOR( AParticleEffect, Actor )

public:
	AParticleEffect();
	~AParticleEffect() override;

	void Tick() override;
	void Draw() override;

	ActorSpawn Serialize() override { return ActorSpawn(); }
	void Deserialize( const ActorSpawn &spawn ) override;

protected:
private:
	ParticleEffect *effect;
};

REGISTER_ACTOR_BASIC( AParticleEffect )

AParticleEffect::AParticleEffect() : SuperClass() {}
AParticleEffect::~AParticleEffect() {
	delete effect;
}

void AParticleEffect::Tick() {
	SuperClass::Tick();

	effect->Tick();
}

void AParticleEffect::Draw() {
	effect->Draw();
}

void AParticleEffect::Deserialize( const ActorSpawn &spawn ) {
	SuperClass::Deserialize( spawn );

	// TODO: might need to revise the ActorSpawn structure in order for this to work
}

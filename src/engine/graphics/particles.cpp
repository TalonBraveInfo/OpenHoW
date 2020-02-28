
#include "../engine.h"

#include "particles.h"

void ParticleEffect::Draw() {
	for( auto &i : emitters ) {
		i.second.Draw();
	}
}

void ParticleEffect::Tick() {
	for( auto &i : emitters ) {
		i.second.Tick();
	}
}

void ParticleEmitter::Tick() {
	for ( auto &i : particles ) {
		i.Tick();
	}
}

void ParticleEmitter::Draw() {}

/* OpenHoW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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

#include "pork_engine.h"
#include "pork_formats.h"
#include "pork_particle.h"

#include "client.h"
#include "client_display.h"

/* everything else */

ParticleSystem systems[MAX_PARTICLE_SYSTEMS];
unsigned int num_systems = 0;

/* todo: particle stuff goes here */

void InitParticles(void) {
    LogInfo("initializing particle sub-system...\n");
    memset(systems, 0, sizeof(ParticleSystem) * MAX_PARTICLE_SYSTEMS);
}

ParticleSystem *GetParticleSystemSlot(void) {
    for(unsigned int i = 0; i < num_systems; ++i) {
        if(!systems[i].is_reserved) {
            return &systems[i];
        }
    }

    return NULL;
}

void SimulateParticles(void) {
    for(unsigned int i = 0; i < num_systems; ++i) {
        if(!systems[i].is_enabled) {
            continue;
        }
    }
}

void DrawParticles(double delta) {
    for(unsigned int i = 0; i < num_systems; ++i) {
        DrawParticleSystem(&systems[i], delta);
    }
}

void DrawParticleSystem(ParticleSystem *ps, double delta) {
    pork_assert(ps != NULL);

    if(!ps->is_visible || !ps->is_reserved) {
        return;
    }

    for(unsigned int i = 0; i < ps->num_emitters; ++i) {
        PSEmitter *emitter = &ps->emitters[i];
        if(emitter->num_particles >= emitter->max_particles) {
            continue;
        }


    }
}

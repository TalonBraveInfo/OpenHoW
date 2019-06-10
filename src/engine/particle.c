/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include "engine.h"
#include "particle.h"
#include "client.h"

#include "loaders/loaders.h"
#include "graphics/display.h"

PPSFormat pps_docs[512];

ParticleSystem *LoadParticleSystem(const char *path) {
    /* todo, check if it's cached already */


    /* load in the particle system so we can cache it */
    FILE *fp = fopen(u_find(path), "rb");
    if(fp == NULL) {
        LogWarn("failed to load PPS \"%s\", ignoring!\n", path);
        return NULL;
    }

    PPSHeader header;
    if(fread(&header, sizeof(PPSHeader), 1, fp) != 1) {
        LogWarn("failed to load PPS header!\n");
        goto ABORT;
    }

    ABORT:
    u_fclose(fp);
    return NULL;
}

bool WriteParticleSystem() {
    return false;
}

void ClearParticleSystemCache(bool force) {

}

PPSFormat SerializeParticleSystem(ParticleSystem *system) {
    static PPSFormat format;
    memset(&format, 0, sizeof(PPSFormat));

    strcpy((char*)format.header.identifier, PPS_IDENTIFIER);
    strcpy((char*)format.header.version, PPS_VERSION);

    return format;
}

ParticleSystem *DeserializeParticleSystem(PPSFormat pps) {
    return NULL;
}

/************************************************************/

/* todo...
 *  instancing
 *  soft particles
 *  animations
 */

ParticleSystem *systems;
unsigned int num_systems = 0;
unsigned int max_systems = BASE_MAX_PARTICLE_SYSTEMS;

/* todo: particle stuff goes here */

void InitParticles(void) {
    LogInfo("initializing particle sub-system...\n");
    systems = u_alloc(max_systems, sizeof(ParticleSystem), true);
    memset(systems, 0, sizeof(ParticleSystem) * max_systems);
}

ParticleSystem *GetParticleSystemSlot(void) {
    for(unsigned int i = 0; i < num_systems; ++i) {
        if(!systems[i].is_reserved) {
            return &systems[i];
        }
    }

    return NULL;
}

void ClearParticleSystemSlot(ParticleSystem *system) {
    free(system->emitters);
    memset(system, 0, sizeof(ParticleSystem));
}

void SimulateParticles(void) {
    for(unsigned int i = 0; i < num_systems; ++i) {
        ParticleSystem *ps = &systems[i];
        if(!ps->is_enabled) {
            continue;
        }

        for(unsigned int j = 0; j < ps->num_emitters; ++j) {
            PSEmitter *pe = &ps->emitters[j];
            if(pe->num_particles == 0) {
                continue;
            }

        }
    }
}

int SortParticles(const void *a, const void *b) {
    const ParticleSystem *sys0 = (const ParticleSystem*)a;
    const ParticleSystem *sys1 = (const ParticleSystem*)b;
    /* todo, get particle distance to camera and compare the two */
    return 0;
}

void DrawParticles(double delta) {
    qsort(systems, num_systems, sizeof(ParticleSystem), &SortParticles);
    for(unsigned int i = 0; i < num_systems; ++i) {
        DrawParticleSystem(&systems[i], delta);
    }
}

void DrawParticleSystem(ParticleSystem *ps, double delta) {
    u_assert(ps != NULL);

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

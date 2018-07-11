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

#pragma once

#define MAX_PARTICLE_SYSTEMS    2048

typedef enum PSEmitterType {
    PS_EMITTER_TYPE_SPRITE,
    PS_EMITTER_TYPE_MODEL,

    MAX_PS_EMITTER_TYPES
} PSEmitterType;

typedef enum PSBlendType {
    PS_BLEND_TYPE_NONE,
    PS_BLEND_TYPE_ADDITIVE,
    PS_BLEND_TYPE_SUBTRACTIVE,
    PS_BLEND_TYPE_DIFFERENCE,

    MAX_PS_BLEND_TYPES
} PSBlendType;

typedef struct Particle {
    PLVector3 position;
    PLVector3 velocity;
    PLColour colour;
    float size;
} Particle;

typedef struct PSEmitter {
    PLVector3 position;
    PLTexture *texture;

    PSBlendType blend;
    PSEmitterType type;

    Particle *particles;
    unsigned int num_particles;
    unsigned int max_particles;
} PSEmitter;

typedef struct ParticleSystem {
    PSEmitter *emitters;
    unsigned int num_emitters;
    unsigned int max_emitters;

    bool is_reserved;
    bool is_enabled;
    bool is_visible;

    /* editor only */

    char name[32];
} ParticleSystem;

void InitParticles(void);
void SimulateParticles(void);
void DrawParticles(double delta);

void DrawParticleSystem(ParticleSystem *ps, double delta);

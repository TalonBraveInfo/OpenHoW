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

#pragma once

#define BASE_MAX_PARTICLE_SYSTEMS   2048

typedef enum PSEmitterType {
  PS_EMITTER_TYPE_SPRITE,
  PS_EMITTER_TYPE_MODEL,
  PS_EMITTER_TYPE_LINE,

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

/******************************************************************/
/* PPS format */

#define PPS_IDENTIFIER  "PPS\0"
#define PPS_VERSION     "050818"
#define PPS_EXTENSION   "pps"

typedef struct PPSHeader {
  int8_t identifier[4];
  int8_t version[6];
  uint32_t num_chunks;    /* number of chunks following the header (excluding children) */
} PPSHeader;

typedef struct PPSChunkHeader {
#if 1
  int8_t name[16];
#else
  uint16_t identity;
#endif
  uint32_t length;
  uint32_t num_children;
} PPSChunkHeader;

/* Emitter properties
 *  IsLooping       uint8_t
 *  MaxParticles    uint16_t
 *  Material        int8_t[...] (length before?)
 *  StartColour     uint8_t[4]
 *  EndColour       uint8_t[4]
 *  StartPosition   float
 *  EndPosition     float
 *  StartSize       float
 *  EndSize         float
 *  BlendMode       uint8_t
 */

/* Emitter nodes
 *  Light
 *      Colour
 *  Particle
 */

typedef struct PPSPositionChunk {   /* "position" */
  PPSChunkHeader header;
  int32_t start_position[3];
  int32_t end_position[3];
} PPSPositionChunk;

typedef struct PPSColourChunk { /* "colour" */
  PPSChunkHeader header;
  uint8_t start_colour[4];
  uint8_t end_colour[4];
} PPSColourChunk;

typedef struct PPSSizeChunk { /* "size" */
  PPSChunkHeader header;
  int32_t start_size;
  int32_t end_size;
} PPSSizeChunk;

typedef struct PPSEmitterChunk {    /* "emitter" */
  PPSChunkHeader header;
  uint32_t max_particles;
  PPSPositionChunk position;
  PPSColourChunk colour;
  PPSSizeChunk size;
} PPSEmitterChunk;

typedef struct PPSFormat {
  PPSHeader header;
  PPSChunkHeader *chunks;
  uint32_t num_chunks;
} PPSFormat;

PPSFormat SerializeParticleSystem(ParticleSystem *system);
ParticleSystem *DeserializeParticleSystem(PPSFormat pps);

/******************************************************************/

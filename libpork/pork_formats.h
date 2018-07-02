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

/******************************************************************/
/* Pork Particle System */

typedef struct PPSHeader {
    int8_t identifier[4];   /* "PPS " */
    int8_t version[4];      /* "1   " */
    uint32_t num_chunks;    /* number of chunks following the header (excluding children) */
} PPSHeader;

typedef struct PPSChunkHeader {
    int8_t name[16];
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

typedef struct PPSPositionChunk {   /* "Position" */
    PPSChunkHeader header;
    int32_t position[3];
} PPSPositionChunk;

/******************************************************************/
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

#define MAP_MODE_DEATHMATCH         (unsigned int)(1 << 0)
#define MAP_MODE_SINGLEPLAYER       (unsigned int)(1 << 1)
#define MAP_MODE_SURVIVAL_NOVICE    (unsigned int)(1 << 2)
#define MAP_MODE_SURVIVAL_EXPERT    (unsigned int)(1 << 3)
#define MAP_MODE_SURVIVAL_STRATEGY  (unsigned int)(1 << 4)
#define MAP_MODE_GENERATED          (unsigned int)(1 << 5)
#define MAP_MODE_TRAINING           (unsigned int)(1 << 6)
#define MAP_MODE_EDITOR             (unsigned int)(1 << 7)

#define MAP_CHUNK_ROW           16
#define MAP_CHUNKS              (MAP_CHUNK_ROW * MAP_CHUNK_ROW)
#define MAP_CHUNK_ROW_TILES     4
#define MAP_CHUNK_TILES         (MAP_CHUNK_ROW_TILES * MAP_CHUNK_ROW_TILES)

#define MAP_TILE_PIXEL_WIDTH    512
#define MAP_CHUNK_PIXEL_WIDTH   2048

#define MAP_ROW_TILES           (MAP_CHUNK_ROW * MAP_CHUNK_ROW_TILES)

#define MAP_PIXEL_WIDTH         (MAP_TILE_PIXEL_WIDTH * MAP_ROW_TILES)

enum {
    TILE_TYPE_MUD       = 0,
    TILE_TYPE_GRASS     = 1,
    TILE_TYPE_METAL     = 2,
    TILE_TYPE_WOOD      = 3,
    TILE_TYPE_WATER     = 4,
    TILE_TYPE_STONE     = 5,
    TILE_TYPE_ROCK      = 6,
    TILE_TYPE_SAND      = 7,
    TILE_TYPE_ICE       = 8,
    TILE_TYPE_SNOW      = 9,
    TILE_TYPE_QUAGMIRE  = 10,
    TILE_TYPE_LAVA      = 11,

    MAX_TILE_TYPES,
};

#define TILE_FLAG_WATERY    32
#define TILE_FLAG_MINE      64
#define TILE_FLAG_WALL      128

void InitMaps(void);
void ShutdownMaps(void);

void UnloadMap(void);
void ResetMap(void);
bool LoadMap(const char *name, unsigned int mode);
void DrawMap(void);

const char *GetCurrentMapName(void);           // returns the name of the current map.
const char *GetCurrentMapDescription(void);    // returns the description of the current map.
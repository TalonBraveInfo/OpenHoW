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

#include "MapManager.h"

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

#define MAP_FLIP_FLAG_X             1
#define MAP_FLIP_FLAG_ROTATE_90     2
#define MAP_FLIP_FLAG_ROTATE_180    4
#define MAP_FLIP_FLAG_ROTATE_270    6

/* format data */

struct MapSpawn {
    char        name[16];               // class name
    char        unused0[16];

    int16_t     position[3];            // position in the world
    uint16_t    index;                  // todo
    int16_t     angles[3];              // angles in the world
    uint16_t    type;                   // todo

    int16_t     bounds[3];              // collision bounds
    uint16_t    bounds_type;            // box, prism, sphere and none

    int16_t     energy;
    uint8_t     appearance;
    uint8_t     team;                   // uk, usa, german, french, japanese, soviet

    uint16_t    objective;
    uint8_t     objective_actor_id;
    uint8_t     objective_extra[2];

    uint8_t     unused1;
    uint16_t    unused2[8];

    int16_t     fallback_position[3];
    int16_t     extra;
    int16_t     attached_actor_num;
    int16_t     unused3;
};
static_assert(sizeof(MapSpawn) == 94, "Invalid size for MapSpawn, should be 94 bytes!");

struct MapTile {
    /* surface properties */
    unsigned int type{0};   /* e.g. wood? */
    unsigned int flags{0};  /* e.g. mine, watery */
    unsigned int slip{0};   /* e.g. full, bottom or left? */

    /* texture */
    unsigned int tex{0};
    unsigned int flip{0};

    float height[4]{0, 0, 0, 0};
};

struct MapChunk {
    MapTile     tiles[16];
    PLVector3   offset{0, 0, 0};
    PLModel     *model{nullptr};
};

struct MapManifest;

/* end format data */

class Map {
public:
    explicit Map(const std::string &name);
    ~Map();

    void Draw();

    float GetMaxHeight() { return max_height_; }
    float GetMinHeight() { return min_height_; }

    const std::string &GetName() { return manifest_->name; }
    const std::string &GetDescription() { return manifest_->description; }

    MapChunk *GetChunk(const PLVector2 &pos);
    MapTile *GetTile(const PLVector2 &pos);

    float GetHeight(const PLVector2 &pos);

    const PLTexture *GetOverviewTexture() { return overview_; }

    const std::vector<MapSpawn> &GetSpawns() { return spawns_; }

protected:
private:
    void LoadSpawns(const std::string &path);
    void LoadTiles(const std::string &path);
    void LoadTextures(const std::string &path);

    void GenerateOverview();

    float max_height_{0};
    float min_height_{0};

    const MapManifest *manifest_{nullptr};

    std::vector<MapChunk> chunks_;
    std::vector<MapSpawn> spawns_;

    PLTexture *overview_{nullptr};

    std::vector<PLTexture*> tile_textures_;

    PLModel *sky_model_{nullptr};
    PLTexture *sky_textures_[4]{nullptr, nullptr, nullptr, nullptr};
};

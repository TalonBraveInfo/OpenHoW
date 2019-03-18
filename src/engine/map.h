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

#define MAP_FLIP_FLAG_X             1
#define MAP_FLIP_FLAG_ROTATE_90     2
#define MAP_FLIP_FLAG_ROTATE_180    4
#define MAP_FLIP_FLAG_ROTATE_270    6

/* format data */

typedef struct MapSpawn { /* this should be 94 bytes */
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
} MapSpawn;

typedef struct MapTile {
    /* surface properties */
    unsigned int type{0};  /* e.g. wood? */
    unsigned int slip{0};  /* e.g. full, bottom or left? */

    /* texture */
    unsigned int tex{0};
    unsigned int flip{0};
} MapTile;

typedef struct MapChunk {
    MapTile tiles[16];

    struct {
        int16_t height{0};
    } vertices[25];

    PLVector3 offset{0, 0, 0};
} MapChunk;

/* end format data */

typedef struct MapManifest {
    std::string     name;
    std::string     path;
    std::string     description;
    std::string     sky;
    unsigned int    flags;
} MapManifest;

class Map {
public:
    Map(const std::string &name, const GameModeSetup &mode);
    ~Map();

    void Reset();
    void Draw();

    float GetMaxHeight() { return max_height_; }
    float GetMinHeight() { return min_height_; }

    const GameModeSetup *GetMode() { return &mode_; }

    const std::string &GetName() { return name_; }
    const std::string &GetDescription() { return description_; }

    MapChunk *GetChunk(const PLVector2 *pos);
    MapTile *GetTile(const PLVector2 *pos);
    float GetHeight(const PLVector2 *pos);

    const PLTexture *GetOverviewTexture() { return overview_; }

protected:
private:
    void LoadSpawns(const std::string &path);
    void LoadTiles(const std::string &path);
    void LoadTextures(const std::string &path);

    void GenerateOverview();

    std::string name_{"none"};
    std::string description_{"none"};
    std::string sky_{"none"};

    GameModeSetup mode_;

    float max_height_{0};
    float min_height_{0};

    std::vector<MapChunk> chunks_;
    std::vector<MapSpawn> spawns_;

    PLTexture *overview_{nullptr};

    std::vector<PLTexture*> tile_textures_;

    PLModel *sky_model_{nullptr};
    PLTexture *sky_textures_[4];
};

PL_EXTERN_C

void CacheMapData(void);

bool Map_Load(const char *name, unsigned int mode);

const MapManifest *Map_GetMapList(unsigned int *num);

PL_EXTERN_C_END

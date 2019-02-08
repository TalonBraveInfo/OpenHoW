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

#include <PL/platform_filesystem.h>
#include <PL/platform_graphics_camera.h>

#include "engine.h"
#include "map.h"
#include "model.h"
#include "game.h"

#include "script/script.h"

#include "client/frontend.h"
#include "client/display.h"

#include "server/actor.h"

#if 0
/* for now these are hard-coded, but
 * eventually we'll do this through a
 * script instead */
MapManifest map_descriptors[]={
        // Single-player

        //{"camp", "Boot camp", MAP_MODE_SINGLEPLAYER},
        {"estu", "The War Foundation", MAP_MODE_SINGLEPLAYER},
        {"road", "Routine Patrol", MAP_MODE_SINGLEPLAYER},
        {"trench", "Trench Warfare", MAP_MODE_SINGLEPLAYER},
        {"devi", "Morning Glory!", MAP_MODE_SINGLEPLAYER},
        {"rumble", "Island Invasion", MAP_MODE_SINGLEPLAYER},
        {"zulus", "Under Siege", MAP_MODE_SINGLEPLAYER},
        {"twin", "Communication Breakdown", MAP_MODE_SINGLEPLAYER},
        {"sniper", "The Spying Game", MAP_MODE_SINGLEPLAYER},
        {"mashed", "The Village People", MAP_MODE_SINGLEPLAYER},
        {"guns", "Bangers 'N' Mash", MAP_MODE_SINGLEPLAYER},
        {"liberate", "Saving Private Rind", MAP_MODE_SINGLEPLAYER},
        {"oasis", "Just Deserts", MAP_MODE_SINGLEPLAYER},
        {"fjords", "Glacier Guns", MAP_MODE_SINGLEPLAYER},
        {"eyrie", "Battle Stations", MAP_MODE_SINGLEPLAYER},
        {"bay", "Fortified Swine", MAP_MODE_SINGLEPLAYER},
        {"medix", "Over The Top", MAP_MODE_SINGLEPLAYER},
        {"bridge", "Geneva Convention", MAP_MODE_SINGLEPLAYER},
        {"desval", "I Spy...", MAP_MODE_SINGLEPLAYER},
        {"snake", "Chemical Compound", MAP_MODE_SINGLEPLAYER},
        {"emplace", "Achilles Heal", MAP_MODE_SINGLEPLAYER},
        {"supline", "High And Dry", MAP_MODE_SINGLEPLAYER},
        {"keep", "Assassination", MAP_MODE_SINGLEPLAYER},
        {"tester", "Hero Warship", MAP_MODE_SINGLEPLAYER},
        {"foot", "Hamburger Hill", MAP_MODE_SINGLEPLAYER},
        {"final", "Well, Well, Well!", MAP_MODE_SINGLEPLAYER},

        // Multi-player

        {"iceflow", "Ice-Flow", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        //{"archi", "You Hillock", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"dbowl", "Death Bowl", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"mlake", "Frost Fight", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"lake", "The Lake", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"ice", "Chill Hill", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"sepia1", "Square Off", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"oneway", "One Way System", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"ridge", "Ridge Back", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
        {"island", "Island Hopper", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},

        {"play1", "Play Pen", MAP_MODE_SURVIVAL_NOVICE},
        {"play2", "Duvet Fun", MAP_MODE_SURVIVAL_NOVICE},
        {"lunar1", "Pigs in Space", MAP_MODE_SURVIVAL_NOVICE},
        {"hell3", "Skulduggery", MAP_MODE_SURVIVAL_NOVICE},
        {"hell2", "Pigin' Hell", MAP_MODE_SURVIVAL_NOVICE},
        {"creepy2", "Graveyard Shift", MAP_MODE_SURVIVAL_NOVICE},

        {"boom", "Friendly Fire", MAP_MODE_SURVIVAL_STRATEGY},
        {"lecprod", "Bridge The Gap", MAP_MODE_SURVIVAL_STRATEGY},
        {"bhill", "Dam Busters", MAP_MODE_SURVIVAL_STRATEGY},
        {"bute", "Moon Buttes", MAP_MODE_SURVIVAL_STRATEGY},
        {"maze", "Hedge Maze", MAP_MODE_SURVIVAL_STRATEGY},
        {"cmass", "Cratermass", MAP_MODE_SURVIVAL_STRATEGY},
        {"artgun", "Doomed", MAP_MODE_SURVIVAL_STRATEGY},
        {"dval", "Death Valley", MAP_MODE_SURVIVAL_STRATEGY},
        {"dval2", "Death Valley 2", MAP_MODE_SURVIVAL_STRATEGY},

        {"demo", NULL, 0},
        {"demo2", NULL, 0},
        {"easy", NULL, 0},

        {"genbrack", NULL, MAP_MODE_GENERATED},
        {"genchalk", NULL, MAP_MODE_GENERATED},
        {"gendesrt", NULL, MAP_MODE_GENERATED},
        {"genlava", NULL, MAP_MODE_GENERATED},
        {"genmud", NULL, MAP_MODE_GENERATED},
        {"gensnow", NULL, MAP_MODE_GENERATED},

        {"hillbase", NULL, 0},

        {"airaid", NULL, 0},
        {"canal", NULL, 0},
        {"climb", NULL, 0},
        {"faceoff", NULL, 0},
        {"hell1", NULL, 0},
        {"hiroad", NULL, 0},
        {"hwd", NULL, 0},
        {"newsnu", NULL, 0},
        {"pdrag", NULL, 0},
        {"quack", NULL, 0},
        {"tdd", NULL, 0},
};
#endif

typedef struct MapDesc {
    char name[16];
    char path[PL_SYSTEM_MAX_PATH];
    char description[64];
    char sky[32];
    unsigned int flags;
} MapManifest;

static MapManifest *map_descriptors = NULL;
static unsigned int num_maps = 0;
static unsigned int max_maps = 2048;

MapManifest *Map_GetMapManifest(const char *name) {
    const char *ext = plGetFileExtension(name);
    if(!plIsEmptyString(ext)) {
        for(unsigned int i = 0; i < num_maps; ++i) {
            if(strncmp(map_descriptors[i].path, name, sizeof(map_descriptors[i].path)) == 0) {
                return &map_descriptors[i];
            }
        }
    } else {
        for (unsigned int i = 0; i < num_maps; ++i) {
            if (strncmp(map_descriptors[i].name, name, sizeof(map_descriptors[i].name)) == 0) {
                return &map_descriptors[i];
            }
        }
    }

    LogWarn("failed to get descriptor for \"%s\"!\n", name);
    return NULL;
}

unsigned int Map_GetModeFlag(const char *str) {
    if (pl_strncasecmp("dm", str, 2) == 0) {
        return MAP_MODE_DEATHMATCH;
    } else if (pl_strncasecmp("sp", str, 2) == 0) {
        return MAP_MODE_SINGLEPLAYER;
    } else if(pl_strncasecmp("se", str, 2) == 0) {
        return MAP_MODE_SURVIVAL_EXPERT;
    } else if(pl_strncasecmp("sn", str, 2) == 0) {
        return MAP_MODE_SURVIVAL_NOVICE;
    } else if(pl_strncasecmp("ss", str, 2) == 0) {
        return MAP_MODE_SURVIVAL_STRATEGY;
    } else if(pl_strncasecmp("ge", str, 2) == 0) {
        return MAP_MODE_GENERATED;
    } else if(pl_strncasecmp("tm", str, 2)) {
        /* special case, used for CAMP in original
         * campaign */
        return MAP_MODE_TRAINING;
    } else if(pl_strncasecmp("edit", str, 4) == 0) {
        return MAP_MODE_EDITOR;
    } else {
        LogWarn("unknown mode type %s, ignoring!\n", str);
        return MAP_MODE_DEATHMATCH;
    }
}

void Map_Register(const char *path) {
    LogInfo("registering %s\n", path);

    FILE *fp = fopen(path, "r");
    if(fp == NULL) {
        LogWarn("failed to open map description, %s\n", path);
        return;
    }

    size_t length = plGetFileSize(path);
    if(length > 1024) {
        fclose(fp);
        LogWarn("map description for %s has exceeded hard-limit of 1024 (%d), aborting!\n", path, length);
        return;
    } else if(length <= 1) {
        fclose(fp);
        LogWarn("map description for %s is of an invalid size, %d, aborting!\n", path, length);
        return;
    }

    char buf[length + 1];
    if(fread(buf, sizeof(char), length, fp) != length) {
        LogWarn("failed to read entirety of map description for %s!\n", path);
    }
    fclose(fp);

    buf[length] = '\0';

    if((num_maps + 1) >= max_maps) {
        LogDebug("reached maximum maps (%u) - allocating more slots...\n", max_maps);

        MapManifest *old_desc = map_descriptors;
        unsigned int old_max_maps = max_maps;

        max_maps += 128;
        if((map_descriptors = realloc(map_descriptors, sizeof(MapManifest) * max_maps)) == NULL) {
            LogWarn("failed to resize map descriptors array to %u bytes!\n", sizeof(MapManifest) * max_maps);
            map_descriptors = old_desc;
            max_maps = old_max_maps;
            num_maps--;
            return;
        }

        memset(map_descriptors + old_max_maps, 0, sizeof(MapManifest) * (max_maps - old_max_maps));
    }

    MapManifest *slot = &map_descriptors[num_maps];
    memset(slot, 0, sizeof(MapManifest));

    LogDebug("%s\n", buf);

    strncpy(slot->path, path, sizeof(slot->path));
    LogDebug("%s\n", slot->path);

    ScriptContext *ctx = Script_CreateContext();
    Script_ParseBuffer(ctx, buf);

    plStripExtension(slot->name, sizeof(slot->name), plGetFileName(path));

    strncpy(slot->description, Script_GetStringProperty(ctx, "name", ""), sizeof(slot->description));
    strncpy(slot->sky, Script_GetStringProperty(ctx, "sky", ""), sizeof(slot->sky));

    ScriptArray *array = Script_GetArrayStrings(ctx, "modes");
    if(array != NULL) {
        for(unsigned int i = 0; i < array->num_strings; ++i) {
            slot->flags |= Map_GetModeFlag(array->strings[i].data);
        }
    }

    Script_DestroyArrayStrings(array);
    Script_DestroyContext(ctx);

    /* todo, the above is fucked, fix it you fool! */
    /* temp */ slot->flags |= MAP_MODE_DEATHMATCH;

    printf("flags = %d\n", slot->flags);

    num_maps++;
}

////////////////////////////////////////////////////////////////

#define FLIP_FLAG_X             1
#define FLIP_FLAG_ROTATE_90     2
#define FLIP_FLAG_ROTATE_180    4
#define FLIP_FLAG_ROTATE_270    6

typedef struct ActorSpawn { /* this should be 94 bytes */
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
} ActorSpawn;

typedef struct MapTile {
    /* surface properties */
    unsigned int type;  /* e.g. wood? */
    unsigned int slip;  /* e.g. full, bottom or left? */

    /* texture */
    unsigned int tex;
    unsigned int flip;
} MapTile;

typedef struct MapChunk {
    MapTile tiles[16];

    struct {
        int16_t height;
    } vertices[25];

    PLVector3 offset;
} MapChunk;

struct {
    char name[24];
    char desc[256];

    MapChunk *chunks;
    unsigned int num_chunks;

    unsigned int flags;

    ActorSpawn *spawns;
    unsigned int num_spawns;

    PLTexture **textures;
    unsigned int num_textures;

    PLTexture *overview;

    PLTexture *sky_textures[4];
    PLModel *sky_model;

    uint height_bounds[2];
} map_state;

const char *Map_GetCurrentName(void) {
    return &map_state.name[0];
}

const char *Map_GetCurrentDescription(void) {
    const char *name = Map_GetCurrentName();
    if(name[0] == '\0') {
        LogWarn("attempted to get a map description without loading a map!\n");
        return "no map loaded";
    }

    MapManifest *desc = Map_GetMapManifest(name);
    if(desc == NULL) {
        return "";
    }

    return &desc->description[0];
}

PLMesh *terrain_mesh = NULL;

/****************************************************/
/* WATER                                            */

#define WATER_WIDTH         256                         // width of water area
#define WATER_HEIGHT        256                         // height of water area
#define WATER_TILES         WATER_WIDTH * WATER_HEIGHT
#define WATER_TILE_WIDTH    16
#define WATER_TILE_HEIGHT   16

PLTexture *water_textures[2];
PLMesh *water_mesh = NULL;

typedef struct WaterTile {
    PLVector3 position;
} WaterTile;
WaterTile water_tiles[WATER_TILES];

static void GenerateWaterTiles(void) {
    water_textures[0] = Display_LoadTexture("textures/wat01", PL_TEXTURE_FILTER_MIPMAP_LINEAR);
    water_textures[1] = Display_LoadTexture("textures/wat02", PL_TEXTURE_FILTER_MIPMAP_LINEAR);

    water_mesh = plCreateMesh(PL_MESH_TRIANGLES, PL_DRAW_STATIC, 2, 4);
    if (water_mesh == NULL) {
        Error("failed to create water mesh, %s, aborting!\n", plGetError());
    }

    plClearMesh(water_mesh);

    plSetMeshUniformColour(water_mesh, PL_COLOUR_WHITE);

    plSetMeshVertexPosition(water_mesh, 0, PLVector3(-1, -1, -1));
    plSetMeshVertexPosition(water_mesh, 1, PLVector3(1, 1, -1));
    plSetMeshVertexPosition(water_mesh, 2, PLVector3(1, -1, -1));
    plSetMeshVertexPosition(water_mesh, 3, PLVector3(1, 1, 1));

    plUploadMesh(water_mesh);

#define water_tile_pos  (y * WATER_TILE_WIDTH) + x
    memset(water_tiles, 0, sizeof(WaterTile) * WATER_TILES);
    unsigned int x_pos = 0, y_pos = 0;
    for(unsigned int y = 0; y < WATER_WIDTH; ++y) {
        for(unsigned int x = 0; x < WATER_HEIGHT; ++x) {
            water_tiles[water_tile_pos].position.x = x_pos;
            water_tiles[water_tile_pos].position.y = y_pos;
            x_pos += WATER_TILE_WIDTH;
        }
        y_pos += WATER_TILE_HEIGHT;
        x_pos = 0;
    }
}

/****************************************************/

/* possible optimisations...
 *  a) tiles that use the same texture are part of the same mesh instance
 *  b) convert all tiles into a single texture, upload to GPU - using all tiles as single mesh
 *  c) keep all tiles separate, apply simple ray tracing algorithm to check whether tile is visible from camera
 */

void MapCommand(unsigned int argc, char *argv[]) {
    if(argc < 2) {
        LogWarn("invalid number of arguments, ignoring!\n");
        return;
    }

    const char *map_name = argv[1];
    unsigned int mode = MAP_MODE_DEATHMATCH;
    if(argc > 2) {
        const char *cmd_mode = argv[2];
        mode = Map_GetModeFlag(cmd_mode);
    }

    StartGame(map_name, mode, 1, true);
}

void MapsCommand(unsigned int argc, char *argv[]) {
    if(map_descriptors == NULL) {
        Error("attempted to list indexed maps before index generated, aborting!\n");
    }

    for(unsigned int i = 0; i < num_maps; ++i) {
        char str[128];
        snprintf(str, sizeof(str), "%s : %s : %u\n",
                 map_descriptors[i].name,
                 map_descriptors[i].description,
                 map_descriptors[i].flags
        );
        LogInfo("%s", str);
    }
    LogInfo("%u maps\n", num_maps);
}

void InitMaps(void) {
    /* register all of the existing maps */

    map_descriptors = u_alloc(max_maps, sizeof(MapManifest), true);

    char map_path[PL_SYSTEM_MAX_PATH];
    if(!plIsEmptyString(GetCampaignPath())) {
        snprintf(map_path, sizeof(map_path), "%s/campaigns/%s/maps", GetBasePath(), GetCampaignPath());
        plScanDirectory(map_path, "map", Map_Register, false);
    }
    snprintf(map_path, sizeof(map_path), "%s/maps", GetBasePath());
    plScanDirectory(map_path, "map", Map_Register, false);

    map_state.sky_model = LoadModel("skys/skydome", true);

    /* generate base meshes */

    GenerateWaterTiles();

    /* register console commands */

    plRegisterConsoleCommand("map", MapCommand, "Changes the current level to whatever is specified");
    plRegisterConsoleCommand("maps", MapsCommand, "Lists all the indexed maps");
}

/* unloads the current map from memory */
void Map_Unload(void) {
    if(map_state.name[0] == '\0') {
        return;
    }

    if(map_state.num_textures > 0) {
        LogDebug("freeing %u textures...\n", map_state.num_textures);
        for(unsigned int i = 0; i < map_state.num_textures; ++i) {
            if(map_state.textures[i] == NULL) {
                break;
            }

            plDeleteTexture(map_state.textures[i], true);
            map_state.textures[i] = NULL;
        }

        u_free(map_state.textures);
        map_state.num_textures = 0;
    }

    u_free(map_state.spawns);
    memset(&map_state, 0, sizeof(map_state));
}

/* resets the state of the map to how
 * it was when it was first loaded into
 * memory */
void Map_ResetState(void) {
    if(map_state.name[0] == '\0') {
        LogWarn("attempted to reset map, but no map is currently loaded, aborting\n");
        return;
    }

#if 0 /* todo: redo */
    SVClearActors();

    for(unsigned int i = 0; i < map_state.num_spawns; ++i) {
        Actor *actor = Actor_Spawn();
        if(actor == NULL) {
            /* warn, and try to keep going for as long as we can :( */
            LogWarn("failed to spawn actor, probably a memory issue? aborting\n");
            break;
        }

        actor->position.x = map_state.spawns[i].position[0];
        actor->position.y = map_state.spawns[i].position[1];
        actor->position.z = map_state.spawns[i].position[2];

        actor->angles.x   = map_state.spawns[i].angles[0];
        actor->angles.y   = map_state.spawns[i].angles[1];
        actor->angles.z   = map_state.spawns[i].angles[2];

        //actor->bounds   = map_state.spawns[i].bounds;
        //actor->bounds   = map_state.spawns[i].bounds;
        //actor->bounds   = map_state.spawns[i].bounds;

        actor->team = map_state.spawns[i].team;

        strncpy(actor->name, map_state.spawns[i].name, sizeof(actor->name));
        /* todo, setup the actor here - name represents class - this will then
         * set the model and other parms here
         */

        //actor->logic.spawn_delay = map_state.spawns[i].spawn_delay;
    }
#endif
}

static MapChunk *GetChunkAtPosition(const PLVector2 *pos) {
    uint idx = ((uint)(pos->x) / MAP_CHUNK_PIXEL_WIDTH) + (((uint)(pos->y) / MAP_CHUNK_PIXEL_WIDTH) * MAP_CHUNK_ROW);
    if(idx >= map_state.num_chunks) {
        LogWarn("attempted to get an out of bounds chunk index!\n");
        return NULL;
    }

    //LogDebug("chunk idx = %d\n", idx);

    return &map_state.chunks[idx];
}

static MapTile *GetTileAtPosition(const PLVector2 *pos) {
    MapChunk *chunk = GetChunkAtPosition(pos);
    if(chunk == NULL) {
        return NULL;
    }

    uint idx = (((uint)(pos->x) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) +
               ((((uint)(pos->y) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) * MAP_CHUNK_ROW_TILES);
    if(idx >= MAP_CHUNK_TILES) {
        LogWarn("attempted to get an out of bounds tile index!\n");
        return NULL;
    }

    LogDebug("tile idx = %d\n", idx);
    LogDebug("tile x = %d ",(((uint)(pos->x) / MAP_TILE_PIXEL_WIDTH)) );
    LogDebug("tile y = %d\n", (((uint)(pos->y) / MAP_TILE_PIXEL_WIDTH) ));

    return &chunk->tiles[idx];
}

static void LoadMapSpawns(const char *path) {
    FILE *fh = fopen(path, "rb");
    if(fh == NULL) {
        Error("failed to open actor data, \"%s\", aborting\n", path);
    }

    uint16_t num_indices;
    if(fread(&num_indices, sizeof(uint16_t), 1, fh) != 1) {
        Error("failed to get number of indices from pog, \"%s\", aborting\n", path);
    }

    map_state.spawns = u_alloc(num_indices, sizeof(*map_state.spawns), true);
    map_state.num_spawns = num_indices;
    for(unsigned int i = 0; i < num_indices; ++i) {
        if(fread(&map_state.spawns[i], sizeof(ActorSpawn), 1, fh) != 1) {
            Error("failed to load index %d from POG, \"%s\"!\n", i, path);
        }

#define v(a) map_state.spawns[i].a[0], map_state.spawns[i].a[1], map_state.spawns[i].a[2]
        LogDebug("name %s\n", map_state.spawns[i].name);
        LogDebug("position %d %d %d\n", v(position));
        LogDebug("bounds %d %d %d\n", v(bounds));
        LogDebug("angles %d %d %d\n", v(angles));
        LogDebug("fallback %d %d %d\n", v(fallback_position));
    }

    u_fclose(fh);
}

static void LoadMapTiles(const char *path) {
    LogDebug("loading map tiles...\n");

    FILE *fh = fopen(path, "rb");
    if(fh == NULL) {
        Error("failed to open tile data, \"%s\", aborting\n", path);
    }

    /* done here in case the enhanced format supports larger chunk sizes */
    map_state.num_chunks = MAP_CHUNKS;
    map_state.chunks = u_alloc(sizeof(MapChunk), map_state.num_chunks, true);

    LogDebug("%u chunks for terrain\n", map_state.num_chunks);

    memset(map_state.chunks, 0, sizeof(MapChunk) * map_state.num_chunks);

    for(unsigned int chunk_y = 0; chunk_y < MAP_CHUNK_ROW; ++chunk_y) {
        for(unsigned int chunk_x = 0; chunk_x < MAP_CHUNK_ROW; ++chunk_x) {
#define CUR_CHUNK map_state.chunks[chunk_x + chunk_y * MAP_CHUNK_ROW]

            struct __attribute__((packed)) {
                /* offsets */
                uint16_t x;
                uint16_t y;
                uint16_t z;

                uint16_t unknown0;
            } chunk;
            if(fread(&chunk, sizeof(chunk), 1, fh) != 1) {
                Error("unexpected end of file, aborting!\n");
            }
            CUR_CHUNK.offset = PLVector3(chunk.x, chunk.y, chunk.z);

            for(unsigned int vertex_y = 0; vertex_y < 5; ++vertex_y) {
                for(unsigned int vertex_x = 0; vertex_x < 5; ++vertex_x) {
                    struct __attribute__((packed)) {
                        int16_t     height;
                        uint16_t    lighting;
                    } vertex;
                    if(fread(&vertex, sizeof(vertex), 1, fh) != 1) {
                        Error("unexpected end of file, aborting!\n");
                    }
                    CUR_CHUNK.vertices[vertex_x + vertex_y * 5].height = vertex.height;
                }
            }

            fseek(fh, 4, SEEK_CUR);

            for(unsigned int tile_y = 0; tile_y < MAP_CHUNK_ROW_TILES; ++tile_y) {
                for(unsigned int tile_x = 0; tile_x < MAP_CHUNK_ROW_TILES; ++tile_x) {
                    struct __attribute__((packed)) {
#if 0
                        int8_t      unused0[10];
                        uint16_t    info;
                        int8_t      unused1[2];
                        uint8_t     rotation;
                        uint8_t     texture;
#else
                        int8_t      unused0[6];
                        uint8_t     type;
                        uint8_t     slip;
                        int16_t     unused1;
                        uint8_t     rotation;
                        uint32_t    texture;
                        uint8_t     unused2;
#endif
                    } tile;
                    if(fread(&tile, sizeof(tile), 1, fh) != 1) {
                        Error("unexpected end of file, aborting!\n");
                    }

                    /* todo, load texture and apply it to texture atlas
                     * let's do this per-block? i'm paranoid about people
                     * trying to add massive textures for each tile... :(
                     */

                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].type   = (unsigned int) (tile.type & 31);
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].flip   = tile.rotation;
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].slip   = 0;
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].tex    = tile.texture;

                    u_assert(CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].type < MAX_TILE_TYPES,
                                "invalid tile type!\n");

#if 1
                    LogDebug("\ntile %u\n"
                             "  type %u\n",
                             tile_x + tile_y * MAP_CHUNK_ROW_TILES,
                             CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].type
                    );
#endif
                }
            }
        }
    }

    u_fclose(fh);

    if(terrain_mesh != NULL) {
        plDeleteMesh(terrain_mesh);
    }

    terrain_mesh = plCreateMesh(PL_MESH_TRIANGLE_STRIP, PL_DRAW_STATIC, 64, 256);
    if(terrain_mesh == NULL) {
        Error("failed to create terrain mesh, %s, aborting!\n", plGetError());
    }
}

static void LoadMapTextures(MapManifest *desc, const char *path) {

    /* load in the textures we'll be using for the sky dome */

    if(map_state.sky_textures[0] != NULL) {
        for(unsigned int i = 0; i < 4; ++i) {
            plDeleteTexture(map_state.sky_textures[i], true);
            map_state.sky_textures[0] = NULL;
        }
    }

    char sky_path[PL_SYSTEM_MAX_PATH] = { '\0' };
    /* was this even the default in the original!? */
    if(desc->sky[0] != '\0' && desc->sky[0] != ' ') {
        snprintf(sky_path, sizeof(sky_path), "%s%s1", u_find("skys/"), desc->sky);
        if(!plPathExists(sky_path)) {
            LogWarn("failed to find texture path for sky at \"%s\", reverting to default!\n", sky_path);
            sky_path[0] = '\0';
        }
    }

    if(sky_path[0] == '\0') {
        LogInfo("no sky specified, using default\n");
        snprintf(sky_path, sizeof(sky_path), "%s", u_find("skys/sunny/"));
    }

    /*
    FILE *fh = fopen(path, "rb");
    if(fh == NULL) {
        Error("failed to open texture data, \"%s\", aborting\n", path);
    }

    fclose(fh);
     */
}

static void GenerateMinimapTexture(void) {
    if(map_state.overview != NULL) {
        plDeleteTexture(map_state.overview, true);
    }

    static const PLColour colours[MAX_TILE_TYPES]={
        { 60, 50, 40 },     // Mud
        { 40, 70, 40 },     // Grass
        { 128, 128, 128 },  // Metal
        { 153, 94, 34 },    // Wood
        { 90, 90, 150 },    // Water
        { 50, 50, 50 },     // Stone
        { 50, 50, 50 },     // Rock
        { 100, 80, 30 },    // Sand
        { 180, 240, 240 },  // Ice
        { 100, 100, 100 },  // Snow
        { 60, 50, 40 },     // Quagmire
        { 100, 240, 53 }    // Lava/Poison
    };

    /* create our storage */

    PLImage image;
    memset(&image, 0, sizeof(PLImage));
    image.width         = 64;
    image.height        = 64;
    image.format        = PL_IMAGEFORMAT_RGB8;
    image.colour_format = PL_COLOURFORMAT_RGB;
    image.size          = plGetImageSize(image.format, image.width, image.height);
    image.levels        = 1;

    image.data = u_alloc(image.levels, sizeof(uint8_t *), true);
    image.data[0] = u_alloc(1, image.size, true);

    /* now write into the image buffer */

    uint8_t *buf = image.data[0];
    for(uint8_t y = 0; y < 64; ++y) {
        for(uint8_t x = 0; x < 64; ++x) {
            MapTile *tile = GetTileAtPosition(&PLVector2(x * (MAP_PIXEL_WIDTH / 64), y * (MAP_PIXEL_WIDTH / 64)));
            if(tile == NULL) {
                Error("hit an invalid tile during minimap generation!\n");
            }

            *(buf++) = colours[tile->type].r;
            *(buf++) = colours[tile->type].g;
            *(buf++) = colours[tile->type].b;

#if 0
            LogDebug("%d(%d): %d %d %d\n", x * y,
                     tile->type,
                     colours[tile->type].r,
                     colours[tile->type].g,
                     colours[tile->type].b);
#endif
        }
    }

    plWriteImage(&image, "./test.png");

    plFreeImage(&image);
}

/* loads a new map into memory - if the config
 * matches that of the currently loaded map
 * then it's ignored */
bool Map_Load(const char *name, unsigned int mode) {
    LogDebug("loading map, %s, in mode %u\n", name, mode);
    if(map_state.name[0] != '\0') {
        if(strncmp(map_state.name, name, sizeof(map_state.name)) == 0) {
            if(map_state.flags == mode) {
                LogWarn("attempted to load duplicate map, \"%s\", aborting!\n", name);
                return true;
            }

            LogInfo("attempted to load duplicate map, \"%s\", but with alternate state, resetting\n", name);
            Map_ResetState();
            return true;
        }
    }

    MapManifest *desc = Map_GetMapManifest(name);
    if(desc == NULL) {
        /* todo: support maps without .map manifest? */
        LogWarn("failed to get descriptor for map, \"%s\", aborting!\n", name);
        return false;
    }

    //LogDebug("found map description!\n %s\n %s", desc->name, desc->description);

    if(mode != MAP_MODE_EDITOR && !(desc->flags & mode)) {
        LogWarn("the mode you're attempting to use is unsupported by this map, \"%s\", aborting\n", name);
        return false;
    }

    FE_SetLoadingBackground(name);

    char map_name[64];
    snprintf(map_name, sizeof(map_name), "LOADING %s", desc->description);
    pl_strntoupper(map_name, sizeof(map_name));
    FE_SetLoadingProgress(0);
    FE_SetLoadingDescription(map_name);

    Map_Unload();

#if 0
    SetLoadingProgress(5);
    SetLoadingDescription("CHECKING FOR MAP DIRECTORY");

    char map_path[PL_SYSTEM_MAX_PATH];
    snprintf(map_path, sizeof(map_path), "%s/maps/%s/", GetBasePath(), name);
    LogDebug("map: %s\n", map_path);
    if(!plPathExists(map_path)) {
        LogWarn("failed to load map, path \"%s\" doesn't exist, aborting\n", map_path);
        return false;
    }
#endif

    FE_SetLoadingProgress(15);
    FE_SetLoadingDescription("LOADING TILES");

    char path[PL_SYSTEM_MAX_PATH];

    snprintf(path, sizeof(path), "maps/%s/%s.pmg", name, name);
    LogDebug("pmg: %s\n", path);
    const char *p = u_find(path);
    if(!plFileExists(p)) {
        LogWarn("failed to load pmg, file \"%s\" doesn't exist, aborting\n", p);
        return false;
    }

    LoadMapTiles(p);

    GenerateMinimapTexture();

    FE_SetLoadingProgress(20);
    FE_SetLoadingDescription("LOADING OBJECTS");

    snprintf(path, sizeof(path), "maps/%s/%s.pog", name, name);
    LogDebug("pog: %s\n", path);
    p = u_find(path);
    if (!plFileExists(p)) {
        LogWarn("failed to load pog, file \"%s\" doesn't exist, aborting\n", p);
        Map_Unload();
        return false;
    }

    LoadMapSpawns(p);
    LoadMapTextures(desc, NULL);

    strncpy(map_state.name, desc->name, sizeof(map_state.name));

    FE_SetLoadingProgress(30);
    FE_SetLoadingDescription("SPAWNING OBJECTS");

    Map_ResetState();

    FE_SetLoadingProgress(100);
    FE_SetLoadingDescription("FINISHED");

    return true;
}

/****************************************************/
/* Draw Stuff! */

#include "client/font.h"
#include "client/shader.h"

/* draws the currently loaded map */
void Map_Draw(void) {
    if(map_state.name[0] == '\0' || GetFrontendState() != FE_MODE_GAME) {
        return;
    }

    if(map_state.sky_model != NULL) {
        plDrawModel(map_state.sky_model);
    }

    /* todo: translate plane to camera pos - we will have
     * another water plane outside / below without reflections? */

    plSetShaderProgram(programs[SHADER_WATER]);

    for(unsigned int i = 0; i < WATER_TILES; ++i) {
        // todo!!!!
        plTranslateMatrix(water_tiles[i].position);

        plDrawMesh(water_mesh);
    }

    plSetShaderProgram(programs[SHADER_DEFAULT]);

    // todo, clouds

    // todo, update parts of terrain mesh from deformation etc?

    plDrawMesh(terrain_mesh);
}

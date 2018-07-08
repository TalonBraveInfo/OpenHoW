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
#include <PL/platform_filesystem.h>
#include <PL/platform_graphics_camera.h>

#include "pork_engine.h"
#include "pork_map.h"
#include "pork_model.h"

#include "script/script.h"

#include "client/client_frontend.h"
#include "client/client_actor.h"
#include "client/client_display.h"

#include "server/server_actor.h"

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
    char description[64];
    char sky[32];
    unsigned int flags;
} MapManifest;

MapManifest *map_descriptors = NULL;
unsigned int num_maps = 0;
unsigned int max_maps = 2048;

MapManifest *GetMapDesc(const char *name) {
    for(unsigned int i = 0; i < num_maps; ++i) {
        if(strcmp(map_descriptors[i].name, name) == 0) {
            return &map_descriptors[i];
        }
    }

    LogWarn("failed to get descriptor for \"%s\"!\n", name);
    return NULL;
}

unsigned int GetMapModeFlag(const char *str) {
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
    } else {
        LogWarn("unknown mode type %s, ignoring!\n", str);
        return MAP_MODE_DEATHMATCH;
    }
}

void RegisterMap(const char *path) {
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

    ParseJSON(buf);

    plStripExtension(slot->name, plGetFileName(path));

    strncpy(slot->description, GetJSONStringProperty("name"), sizeof(slot->description));
    strncpy(slot->sky, GetJSONStringProperty("sky"), sizeof(slot->sky));
    if(duk_get_prop_string(jsn_context, -1, "modes") && duk_is_array(jsn_context, -1)) {
        unsigned int num_modes = (unsigned int)duk_get_length(jsn_context, -1);
        for(unsigned int i = 0; i < num_modes; ++i) {
            duk_get_prop_index(jsn_context, -1, i);
            {
                const char *mode = duk_get_string(jsn_context, i);
                if(mode != NULL) {
                    printf("mode = %s\n", mode);
                    slot->flags |= GetMapModeFlag(mode);
                }
            }
            duk_pop(jsn_context);
        }
    } else {
        LogWarn("invalid modes array!\n");
    }
    duk_pop(jsn_context);

    /* todo, the above is fucked, fix it you fool! */
    /* temp */ slot->flags |= MAP_MODE_DEATHMATCH;

    FlushJSON();

    printf("flags = %d\n", slot->flags);

    num_maps++;
}

////////////////////////////////////////////////////////////////

#define FLIP_FLAG_X             1
#define FLIP_FLAG_ROTATE_90     2
#define FLIP_FLAG_ROTATE_180    4
#define FLIP_FLAG_ROTATE_270    6

struct {
    char name[24];
    char desc[256];

    struct {
        struct {
            /* surface properties */
            unsigned int type;  /* e.g. wood? */
            unsigned int slip;  /* e.g. full, bottom or left? */

            /* texture */
            unsigned int tex;
            unsigned int flip;
        } tiles[16];

        struct {
            unsigned int height;
        } vertices[10];

        PLVector3 offset;
    } *chunks;
    unsigned int num_chunks;

    unsigned int flags;

    struct {
        char name[16];

        unsigned int type;

        PLVector3 position;
        PLVector3 angles;
        PLVector3 bounds;

        uint16_t health;
        uint8_t team;

        uint16_t spawn_delay;
    } *spawns;
    unsigned int num_spawns;

    PLTexture **textures;
    unsigned int num_textures;

    PLTexture *sky_textures[4];
    PLModel *sky_model;
} map_state;

const char *GetCurrentMapName(void) {
    return &map_state.name[0];
}

const char *GetCurrentMapDescription(void) {
    const char *name = GetCurrentMapName();
    if(name[0] == '\0') {
        LogWarn("attempted to get a map description without loading a map!\n");
        return "no map loaded";
    }

    MapManifest *desc = GetMapDesc(name);
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

void GenerateWaterTiles(void) {
    water_textures[0] = LoadTexture("maps/wat01", PL_TEXTURE_FILTER_MIPMAP_LINEAR);
    water_textures[1] = LoadTexture("maps/wat02", PL_TEXTURE_FILTER_MIPMAP_LINEAR);

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
        if (pl_strncasecmp("dm", cmd_mode, 2) == 0) {
            /* do nothing */
        } else if (pl_strncasecmp("sp", cmd_mode, 2) == 0) {
            mode = MAP_MODE_SINGLEPLAYER;
        } else if(pl_strncasecmp("se", cmd_mode, 2) == 0) {
            mode = MAP_MODE_SURVIVAL_EXPERT;
        } else if(pl_strncasecmp("sn", cmd_mode, 2) == 0) {
            mode = MAP_MODE_SURVIVAL_NOVICE;
        } else if(pl_strncasecmp("ss", cmd_mode, 2) == 0) {
            mode = MAP_MODE_SURVIVAL_STRATEGY;
        } else if(pl_strncasecmp("ge", cmd_mode, 2) == 0) {
            mode = MAP_MODE_GENERATED;
        } else {
            LogWarn("unknown mode type %s, ignoring!\n", cmd_mode);
        }
    }

    LoadMap(map_name, mode);
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

    map_descriptors = pork_alloc(max_maps, sizeof(MapManifest), true);

    char map_path[PL_SYSTEM_MAX_PATH];
    if(!plIsEmptyString(GetModPath())) {
        snprintf(map_path, sizeof(map_path), "%s/mods/%s/maps", GetBasePath(), GetModPath());
        plScanDirectory(map_path, "pmd", RegisterMap, false);
    }
    snprintf(map_path, sizeof(map_path), "%smaps", GetBasePath());
    plScanDirectory(map_path, "pmd", RegisterMap, false);

    map_state.sky_model = LoadModel("chars/sky/skydome", true);

    /* generate base meshes */

    GenerateWaterTiles();

    /* register console commands */

    plRegisterConsoleCommand("map", MapCommand, "Changes the current level to whatever is specified");
    plRegisterConsoleCommand("maps", MapsCommand, "Lists all the indexed maps");
}

void ShutdownMaps(void) {
    UnloadMap();
}

/* unloads the current map from memory */
void UnloadMap(void) {
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

        pork_free(map_state.textures);
        map_state.num_textures = 0;
    }

    pork_free(map_state.spawns);
    memset(&map_state, 0, sizeof(map_state));
}

/* resets the state of the map to how
 * it was when it was first loaded into
 * memory */
void ResetMap(void) {
    if(map_state.name[0] == '\0') {
        LogWarn("attempted to reset map, but no map is currently loaded, aborting\n");
        return;
    }

    if(g_state.is_host) {
        SVClearActors();

        for(unsigned int i = 0; i < map_state.num_spawns; ++i) {
            Actor *actor = Actor_Spawn();
            if(actor == NULL) {
                /* warn, and try to keep going for as long as we can :( */
                LogWarn("failed to spawn actor, probably a memory issue? aborting\n");
                break;
            }

            actor->position = map_state.spawns[i].position;
            actor->bounds   = map_state.spawns[i].bounds;
            actor->angles   = map_state.spawns[i].angles;
            actor->team     = map_state.spawns[i].team;

            strncpy(actor->name, map_state.spawns[i].name, sizeof(actor->name));
            /* todo, setup the actor here - name represents class - this will then
             * set the model and other parms here
             */

            actor->logic.spawn_delay = map_state.spawns[i].spawn_delay;
        }
    }

    ClearClientActors();

    for(unsigned int i = 0; i < map_state.num_spawns; ++i) {
        ClientActor *actor = SpawnClientActor();
        if(actor == NULL) {
            /* warn, and try to keep going for as long as we can :( */
            LogWarn("failed to spawn actor, probably a memory issue? aborting\n");
            break;
        }

        actor->position = map_state.spawns[i].position;
        actor->bounds   = map_state.spawns[i].bounds;
        actor->angles   = map_state.spawns[i].angles;
        actor->team     = map_state.spawns[i].team;

        /* todo, load model... */
        //strncpy(actor->name, map_state.spawns[i].name, sizeof(actor->name));
        /* todo, setup the actor here - name represents class - this will then
         * set the model and other parms here */
    }
}

void LoadMapSpawns(const char *path) {
    FILE *fh = fopen(path, "rb");
    if(fh == NULL) {
        Error("failed to open actor data, \"%s\", aborting\n", path);
    }

    uint16_t num_indices;
    if(fread(&num_indices, sizeof(uint16_t), 1, fh) != 1) {
        Error("failed to get number of indices from pog, \"%s\", aborting\n", path);
    }

    map_state.spawns = pork_alloc(num_indices, sizeof(*map_state.spawns), true);
    map_state.num_spawns = num_indices;
    for(unsigned int i = 0; i < num_indices; ++i) {
        struct __attribute__((packed)) { /* this should be 94 bytes */
            char name[16];
            char unused[16];

            uint16_t offset[3];

            uint16_t id;
            uint16_t unknown0;
            uint16_t angle;
            uint16_t unknown1;

            uint16_t type;

            uint16_t bounds[3];

            uint16_t bridge_flag;

            uint16_t spawn_delay;

            uint8_t unknown2;
            uint8_t team;

            uint16_t event_type;
            uint8_t event_group;
            uint8_t event_parms[19];
            uint16_t event_offset[3];

            uint16_t flag;
            uint16_t turn_delay;
            uint16_t unknown3;
        } index;
        if(fread(&index, sizeof(index), 1, fh) != 1) {
            Error("failed to load index %d from pog, \"%s\"\n", i, path);
        }

        strncpy(map_state.spawns[i].name, pl_strtolower(index.name), sizeof(map_state.spawns[i].name));
        LogDebug("name %s\n", map_state.spawns[i].name);

        map_state.spawns[i].position.x = index.offset[0];
        map_state.spawns[i].position.y = index.offset[1];
        map_state.spawns[i].position.z = index.offset[2];
        LogDebug("position %s\n", plPrintVector3(map_state.spawns[i].position));

        map_state.spawns[i].bounds.x = index.bounds[0];
        map_state.spawns[i].bounds.y = index.bounds[1];
        map_state.spawns[i].bounds.z = index.bounds[2];
        LogDebug("bounds %s\n", plPrintVector3(map_state.spawns[i].bounds));

        map_state.spawns[i].type = index.type;
        map_state.spawns[i].team = index.team;

        if(strncmp("crate", map_state.spawns[i].name, 5) != 0) {
            map_state.spawns[i].health = map_state.spawns[i].spawn_delay;
        } else {
            map_state.spawns[i].spawn_delay = map_state.spawns[i].spawn_delay;
        }

        map_state.spawns[i].angles.y = index.angle;
        LogDebug("angles %s\n", plPrintVector3(map_state.spawns[i].angles));
    }

    pork_fclose(fh);
}

void LoadMapTiles(const char *path) {
    LogDebug("loading map tiles...\n");

    FILE *fh = fopen(path, "rb");
    if(fh == NULL) {
        Error("failed to open tile data, \"%s\", aborting\n", path);
    }

    /* todo, eventually we might want to change this
     * depending on the type of map we're loading in,
     * e.g. extended formats... */
    static const unsigned int block_size = 16;

    /* done here in case the enhanced format supports larger chunk sizes */
    map_state.num_chunks = block_size * block_size;
    map_state.chunks = pork_alloc(sizeof(*map_state.chunks), map_state.num_chunks, true);

    LogDebug("%u chunks for terrain\n", map_state.num_chunks);

    memset(map_state.chunks, 0, sizeof(*map_state.chunks) * map_state.num_chunks);

    for(unsigned int block_y = 0; block_y < block_size; ++block_y) {
        for(unsigned int block_x = 0; block_x < block_size; ++block_x) {
#define CUR_BLOCK map_state.chunks[block_x * block_y + block_x]

            struct __attribute__((packed)) {
                /* offsets */
                uint16_t x;
                uint16_t y;
                uint16_t z;

                uint16_t unknown0;
            } block;
            if(fread(&block, sizeof(block), 1, fh) != 1) {

            }
            CUR_BLOCK.offset = PLVector3(block.x, block.y, block.z);

            for(unsigned int vertex_y = 0; vertex_y < 5; ++vertex_y) {
                for(unsigned int vertex_x = 0; vertex_x < 5; ++vertex_x) {
                    struct __attribute__((packed)) {
                        uint16_t height;
                        uint16_t unknown;
                    } vertex;
                    if(fread(&vertex, sizeof(vertex), 1, fh) != 1) {

                    }
                    CUR_BLOCK.vertices[vertex_x * vertex_y + vertex_x].height = vertex.height;
                }
            }

            fseek(fh, 4, SEEK_CUR);

            for(unsigned int tile_y = 0; tile_y < 4; ++tile_y) {
                for(unsigned int tile_x = 0; tile_x < 4; ++tile_x) {
                    struct __attribute__((packed)) {
                        uint8_t unknown0[6];

                        /* surface properties */
                        uint8_t type;
                        uint16_t slip;

                        uint8_t unknown1;

                        /* texture */
                        uint8_t     rotation_flip;
                        uint32_t    texture_index;

                        uint8_t unknown2;
                    } tile;
                    if(fread(&tile, sizeof(tile), 1, fh) != 1) {

                    }

                    /* todo, load texture and apply it to texture atlas
                     * let's do this per-block? i'm paranoid about people
                     * trying to add massive textures for each tile... :(
                     */

                    CUR_BLOCK.tiles[tile_x * tile_y + block_x].type   = tile.type;
                    CUR_BLOCK.tiles[tile_x * tile_y + block_x].flip   = tile.rotation_flip;
                    CUR_BLOCK.tiles[tile_x * tile_y + block_x].slip   = tile.slip;
                    CUR_BLOCK.tiles[tile_x * tile_y + block_x].tex    = tile.texture_index;

#if 1
                    LogDebug("\ntile %u\n"
                             "  type %u\n"
                             "  flip %u\n"
                             "  slip %u\n"
                             "  tex  %u\n",

                             tile_x * tile_y + block_x,
                             tile.type,
                             tile.rotation_flip,
                             tile.slip,
                             tile.texture_index
                    );
#endif
                }
            }
        }
    }

    pork_fclose(fh);

    if(terrain_mesh != NULL) {
        plDeleteMesh(terrain_mesh);
    }

    terrain_mesh = plCreateMesh(PL_MESH_TRIANGLE_STRIP, PL_DRAW_STATIC, 64, 256);
    if(terrain_mesh == NULL) {
        Error("failed to create terrain mesh, %s, aborting!\n", plGetError());
    }
}

void LoadMapTextures(MapManifest *desc, const char *path) {

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
        snprintf(sky_path, sizeof(sky_path), "%s%s1", pork_find("chars/sky/"), desc->sky);
        if(!plPathExists(sky_path)) {
            LogWarn("failed to find texture path for sky at \"%s\", reverting to default!\n", sky_path);
            sky_path[0] = '\0';
        }
    }

    if(sky_path[0] == '\0') {
        LogInfo("no sky specified, using default\n");
        snprintf(sky_path, sizeof(sky_path), "%s", pork_find("chars/sky/sunny/"));
    }

    /*
    FILE *fh = fopen(path, "rb");
    if(fh == NULL) {
        Error("failed to open texture data, \"%s\", aborting\n", path);
    }

    fclose(fh);
     */
}

/* loads a new map into memory - if the config
 * matches that of the currently loaded map
 * then it's ignored */
void LoadMap(const char *name, unsigned int mode) {
    LogDebug("loading map, %s, in mode %u\n", name, mode);
    if(map_state.name[0] != '\0') {
        if(strncmp(map_state.name, name, sizeof(map_state.name)) == 0) {
            if(map_state.flags == mode) {
                LogWarn("attempted to load duplicate map, \"%s\", aborting!\n", name);
                return;
            }

            LogInfo("attempted to load duplicate map, \"%s\", but with alternate state, resetting\n", name);
            ResetMap();
            return;
        }
    }

    MapManifest *desc = GetMapDesc(name);
    if(desc == NULL) {
        LogWarn("failed to get descriptor for map, \"%s\", aborting!\n", name);
        return;
    }

    //LogDebug("found map description!\n %s\n %s", desc->name, desc->description);

    if(!(desc->flags & mode)) {
        LogWarn("the mode you're attempting to use is unsupported by this map, \"%s\", aborting\n", name);
        return;
    }

    if(!g_state.is_dedicated) {
        SetFrontendState(FE_MODE_LOADING);
        SetLoadingBackground(name);
    }

    char map_name[64];
    snprintf(map_name, sizeof(map_name), "LOADING %s", desc->description);
    pl_strntoupper(map_name, sizeof(map_name));
    SetLoadingProgress(0);
    SetLoadingDescription(map_name);

    UnloadMap();

    SetLoadingProgress(5);
    SetLoadingDescription("CHECKING FOR MAP DIRECTORY");

    char map_path[PL_SYSTEM_MAX_PATH];
    snprintf(map_path, sizeof(map_path), "%s/maps/%s/", GetBasePath(), name);
    LogDebug("map: %s\n", map_path);
    if(!plPathExists(map_path)) {
        LogWarn("failed to load map, path \"%s\" doesn't exist, aborting\n", map_path);
        return;
    }

    SetLoadingProgress(15);
    SetLoadingDescription("LOADING TILES");

    char pmg_path[PL_SYSTEM_MAX_PATH];
    snprintf(pmg_path, sizeof(pmg_path), "%s/%s.pmg", map_path, name);
    LogDebug("pmg: %s\n", pmg_path);
    if(!plFileExists(pmg_path)) {
        LogWarn("failed to load pmg, file \"%s\" doesn't exist, aborting\n", pmg_path);
        return;
    }

    LoadMapTiles(pmg_path);

    SetLoadingProgress(20);
    SetLoadingDescription("LOADING OBJECTS");

    char pog_path[PL_SYSTEM_MAX_PATH];
    snprintf(pog_path, sizeof(pog_path), "%s/%s.pog", map_path, name);
    LogDebug("pog: %s\n", pog_path);
    if (!plFileExists(pog_path)) {
        LogWarn("failed to load pog, file \"%s\" doesn't exist, aborting\n", pog_path);
        UnloadMap();
        return;
    }

    LoadMapSpawns(pog_path);
    LoadMapTextures(desc, NULL);

    strncpy(map_state.name, desc->name, sizeof(map_state.name));

    SetLoadingProgress(30);
    SetLoadingDescription("SPAWNING OBJECTS");

    ResetMap();

    SetLoadingProgress(100);
    SetLoadingDescription("FINISHED");

    if(!g_state.is_dedicated) {
        SetFrontendState(FE_MODE_GAME);
    }
}

/****************************************************/
/* Draw Stuff! */

#include "client/client_font.h"
#include "client/client_shader.h"

//#include <GL/glew.h>

/* draws the currently loaded map */
void DrawMap(void) {
    if(map_state.name[0] == '\0') {
        return;
    }

    if(GetFrontendState() != FE_MODE_GAME && !(GetFrontendState() == FE_MODE_EDITOR &&
                                               g_state.editor.current_context == PORK_ED_CTX_WORLD)) {
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
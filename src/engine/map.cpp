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
#include "script/ScriptConfig.h"

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

/* todo: take better advantage of std */

static MapManifest *map_descriptors = nullptr;
static unsigned int num_maps = 0;
static unsigned int max_maps = 32; /* this will expand, if required */

const MapManifest *Map_GetMapList(unsigned int *num) {
    *num = num_maps;
    return map_descriptors;
}

static MapManifest *GetMapManifest(const std::string &name) {
    const char *ext = plGetFileExtension(name.c_str());
    if(!plIsEmptyString(ext)) {
        for(unsigned int i = 0; i < num_maps; ++i) {
            if(map_descriptors[i].path == name) {
                return &map_descriptors[i];
            }
        }
    } else {
        for (unsigned int i = 0; i < num_maps; ++i) {
            if(map_descriptors[i].name == name) {
                return &map_descriptors[i];
            }
        }
    }

    LogWarn("Failed to get descriptor for \"%s\"!\n", name.c_str());
    return nullptr;
}

static unsigned int GetModeFlag(const char *str) {
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
        LogWarn("Unknown mode type %s, ignoring!\n", str);
        return MAP_MODE_DEATHMATCH;
    }
}

void Map_Register(const char *path) {
    LogInfo("Registering %s\n", path);

    if((num_maps + 1) >= max_maps) {
        LogDebug("Reached maximum maps (%u) - allocating more slots...\n", max_maps);

        MapManifest *old_desc = map_descriptors;
        unsigned int old_max_maps = max_maps;

        max_maps += 128;
        if((map_descriptors = static_cast<MapManifest *>(realloc(map_descriptors, sizeof(MapManifest) * max_maps))) ==
                nullptr) {
            LogWarn("Failed to resize map descriptors array to %u bytes!\n", sizeof(MapManifest) * max_maps);
            map_descriptors = old_desc;
            max_maps = old_max_maps;
            num_maps--;
            return;
        }

        memset(map_descriptors + old_max_maps, 0, sizeof(MapManifest) * (max_maps - old_max_maps));
    }

    MapManifest *slot = &map_descriptors[num_maps];
    memset(slot, 0, sizeof(MapManifest));

    strncpy(slot->path, path, sizeof(slot->path));
    LogDebug("%s\n", slot->path);

    plStripExtension(slot->name, sizeof(slot->name), plGetFileName(path));

    try {
        ScriptConfig config(path);
        strncpy(slot->description, config.GetStringProperty("name").c_str(), sizeof(slot->description));
        strncpy(slot->sky, config.GetStringProperty("sky").c_str(), sizeof(slot->sky));

        std::vector<std::string> modes = config.GetArrayStrings("modes");
        for(const auto &mode : modes) {
            slot->flags |= GetModeFlag(mode.c_str());
        }
    } catch(const std::exception &e) {
        LogWarn("Failed to read map config, \"%s\"!\n%s\n", path, e.what());
    }

    /* todo, the above is fucked, fix it you fool! */
    /* temp */ slot->flags |= MAP_MODE_DEATHMATCH;

    printf("flags = %d\n", slot->flags);

    num_maps++;
}

/************************************************************/

/* possible optimisations...
 *  a) tiles that use the same texture are part of the same mesh instance
 *  b) convert all tiles into a single texture, upload to GPU - using all tiles as single mesh
 *  c) keep all tiles separate, apply simple ray tracing algorithm to check whether tile is visible from camera
 */

void MapCommand(unsigned int argc, char *argv[]) {
    if(argc < 2) {
        LogWarn("Invalid number of arguments, ignoring!\n");
        return;
    }

    GameModeSetup mode;
    memset(&mode, 0, sizeof(GameModeSetup));

    mode.game_mode = MAP_MODE_DEATHMATCH;
    if(argc > 2) {
        const char *cmd_mode = argv[2];
        mode.game_mode = GetModeFlag(cmd_mode);
    }

    mode.num_players = 2;
    mode.teams[0] = TEAM_BRITISH;
    mode.teams[1] = TEAM_AMERICAN;

    snprintf(mode.map, sizeof(mode.map), "%s", argv[1]);

    Game_StartNewGame(&mode);
}

void MapsCommand(unsigned int argc, char *argv[]) {
    if(map_descriptors == nullptr) {
        Error("Attempted to list indexed maps before index generated, aborting!\n");
    }

    for(unsigned int i = 0; i < num_maps; ++i) {
        char str[128];
        snprintf(str, sizeof(str), "%s : %s : %u\n",
                 map_descriptors[i].name.c_str(),
                 map_descriptors[i].description.c_str(),
                 map_descriptors[i].flags
        );
        LogInfo("%s", str);
    }
    LogInfo("%u maps\n", num_maps);
}

void CacheMapData(void) {
    /* register all of the existing maps */

    map_descriptors = static_cast<MapManifest *>(u_alloc(max_maps, sizeof(MapManifest), true));

    char map_path[PL_SYSTEM_MAX_PATH];
    if(!plIsEmptyString(GetCampaignPath())) {
        snprintf(map_path, sizeof(map_path), "%s/campaigns/%s/maps", GetBasePath(), GetCampaignPath());
        plScanDirectory(map_path, "map", Map_Register, false);
    }
    snprintf(map_path, sizeof(map_path), "%s/maps", GetBasePath());
    plScanDirectory(map_path, "map", Map_Register, false);

    sky_model = LoadModel("skys/skydome", true);

    /* generate base meshes */

    //GenerateWaterTiles();

    /* register console commands */

    plRegisterConsoleCommand("map", MapCommand, "Changes the current level to whatever is specified");
    plRegisterConsoleCommand("maps", MapsCommand, "Lists all the indexed maps");
}

static MapChunk *GetChunkAtPosition(const PLVector2 *pos) {
    uint idx = ((uint)(pos->x) / MAP_CHUNK_PIXEL_WIDTH) + (((uint)(pos->y) / MAP_CHUNK_PIXEL_WIDTH) * MAP_CHUNK_ROW);
    if(idx >= map_state.num_chunks) {
        LogWarn("attempted to get an out of bounds chunk index!\n");
        return nullptr;
    }

    //LogDebug("chunk idx = %d\n", idx);

    return &map_state.chunks[idx];
}

static MapTile *GetTileAtPosition(const PLVector2 *pos) {
    MapChunk *chunk = GetChunkAtPosition(pos);
    if(chunk == nullptr) {
        return nullptr;
    }

    uint idx = (((uint)(pos->x) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) +
               ((((uint)(pos->y) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) * MAP_CHUNK_ROW_TILES);
    if(idx >= MAP_CHUNK_TILES) {
        LogWarn("attempted to get an out of bounds tile index!\n");
        return nullptr;
    }

    LogDebug("tile idx = %d\n", idx);
    LogDebug("tile x = %d ",(((uint)(pos->x) / MAP_TILE_PIXEL_WIDTH)) );
    LogDebug("tile y = %d\n", (((uint)(pos->y) / MAP_TILE_PIXEL_WIDTH) ));

    return &chunk->tiles[idx];
}

static float GetHeightAtPosition(const PLVector2 *pos) {
    return 0;
}

static void LoadMapSpawns(const char *path) {
    FILE *fh = fopen(path, "rb");
    if(fh == nullptr) {
        Error("failed to open actor data, \"%s\", aborting\n", path);
    }

    uint16_t num_indices;
    if(fread(&num_indices, sizeof(uint16_t), 1, fh) != 1) {
        Error("failed to get number of indices from pog, \"%s\", aborting\n", path);
    }

    map_state.spawns = static_cast<ActorSpawn *>(u_alloc(num_indices, sizeof(*map_state.spawns), true));
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

static void LoadMapTextures(MapManifest *desc, const char *path) {
    /* free any textures previously loaded */
    for (auto &sky_texture : map_state.sky_textures) {
        if(sky_texture == nullptr) {
            continue;
        }

        plDeleteTexture(sky_texture, true);
        sky_texture = nullptr;
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

Map::Map(const std::string &name, const GameModeSetup &mode) {
    LogDebug("Loading map, %s, in mode %u\n", name.c_str(), mode.game_mode);
    /* map manager now worries about resetting map state etc. */

    MapManifest *desc = GetMapManifest(name);
    if(desc == nullptr) {
        LogWarn("Failed to get map descriptor, %s!\n", name.c_str());
    } else {
        LogDebug("Found map descriptor!\n %s\n %s", desc->name.c_str(), desc->description.c_str());
        if(mode.game_mode != MAP_MODE_EDITOR && !(desc->flags & mode.game_mode)) {
            throw std::runtime_error("This mode is unsupported by this map, " + name + ", aborting!\n");
        }

        name_ = desc->name;
        description_ = desc->description;
        sky_ = desc->sky;
    }

    std::string base_path = "maps/" + name + "/";
    std::string p = u_find(std::string(base_path + name + ".pmg").c_str());
    if(!plFileExists(p.c_str())) {
        throw std::runtime_error("PMG, " + p + ", doesn't exist!\n");
    }

    LoadTiles(p);

    GenerateOverview();

    p = u_find(std::string(base_path + name + ".pog").c_str());
    if (!plFileExists(p.c_str())) {
        throw std::runtime_error("POG, " + p + ", doesn't exist!\n");
    }

    LoadSpawns(p);
    LoadTextures(desc, nullptr);

    Reset();
}

Map::~Map() {
    if(!tile_textures_.empty()) {
        LogDebug("Freeing %u textures...\n", tile_textures_.size());
        for(auto texture : tile_textures_) {
            plDeleteTexture(texture, true);
        }
    }

    if(overview_ != nullptr) {
        plDeleteTexture(overview_, true);
    }

    for (auto &sky_texture : sky_textures_) {
        if(sky_texture == nullptr) {
            break;
        }

        plDeleteTexture(sky_texture, true);
    }
}

void Map::Reset() {
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

void Map::LoadTiles(const std::string &path) {
    FILE *fh = std::fopen(path.c_str(), "rb");
    if(fh == nullptr) {
        Error("Failed to open tile data, \"%s\", aborting\n", path.c_str());
    }

    /* done here in case the enhanced format supports larger chunk sizes */
    chunks_.reserve(MAP_CHUNKS);

    LogDebug("%u chunks for terrain\n", chunks_.max_size());

    for(unsigned int chunk_y = 0; chunk_y < MAP_CHUNK_ROW; ++chunk_y) {
        for(unsigned int chunk_x = 0; chunk_x < MAP_CHUNK_ROW; ++chunk_x) {
#define CUR_CHUNK chunks_[chunk_x + chunk_y * MAP_CHUNK_ROW]

            struct __attribute__((packed)) {
                /* offsets */
                uint16_t x{0};
                uint16_t y{0};
                uint16_t z{0};

                uint16_t unknown0{0};
            } chunk;
            if(std::fread(&chunk, sizeof(chunk), 1, fh) != 1) {
                Error("unexpected end of file, aborting!\n");
            }
            CUR_CHUNK.offset = PLVector3(chunk.x, chunk.y, chunk.z);

            for(unsigned int vertex_y = 0; vertex_y < 5; ++vertex_y) {
                for(unsigned int vertex_x = 0; vertex_x < 5; ++vertex_x) {
                    struct __attribute__((packed)) {
                        int16_t     height{0};
                        uint16_t    lighting{0};
                    } vertex;
                    if(std::fread(&vertex, sizeof(vertex), 1, fh) != 1) {
                        Error("unexpected end of file, aborting!\n");
                    }
                    CUR_CHUNK.vertices[vertex_x + vertex_y * 5].height = vertex.height;
                }
            }

            std::fseek(fh, 4, SEEK_CUR);

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
                        int8_t      unused0[6]{0,0,0,0,0,0};
                        uint8_t     type{0};
                        uint8_t     slip{0};
                        int16_t     unused1{0};
                        uint8_t     rotation{0};
                        uint32_t    texture{0};
                        uint8_t     unused2{0};
#endif
                    } tile;
                    if(std::fread(&tile, sizeof(tile), 1, fh) != 1) {
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

    std::fclose(fh);

#if 0
    if(terrain_mesh != nullptr) {
        plDeleteMesh(terrain_mesh);
    }

    terrain_mesh = plCreateMesh(PL_MESH_TRIANGLE_STRIP, PL_DRAW_STATIC, 64, 256);
    if(terrain_mesh == nullptr) {
        Error("failed to create terrain mesh, %s, aborting!\n", plGetError());
    }
#endif
}

void Map::GenerateOverview() {
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

    image.data = static_cast<uint8_t **>(u_alloc(image.levels, sizeof(uint8_t *), true));
    image.data[0] = static_cast<uint8_t *>(u_alloc(1, image.size, true));

    /* now write into the image buffer */

    uint8_t *buf = image.data[0];
    for(uint8_t y = 0; y < 64; ++y) {
        for(uint8_t x = 0; x < 64; ++x) {
            PLVector2 position(x * (MAP_PIXEL_WIDTH / 64), y * (MAP_PIXEL_WIDTH / 64));
            MapTile *tile = GetTileAtPosition(&position);
            if(tile == nullptr) {
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

void Map::Draw() {
#if 0 /* legacy code */
    if(map_state.name[0] == '\0' || FE_GetState() != FE_MODE_GAME) {
        return;
    }

    if(map_state.sky_model != nullptr) {
        plDrawModel(map_state.sky_model);
    }

    /* todo: translate plane to camera pos - we will have
     * another water plane outside / below without reflections? */

    plSetShaderProgram(programs[SHADER_WATER]);

    for (auto &water_tile : water_tiles) {
        // todo!!!!
        plTranslateMatrix(water_tile.position);

        plDrawMesh(water_mesh);
    }

    plSetShaderProgram(programs[SHADER_DEFAULT]);

    // todo, clouds

    // todo, update parts of terrain mesh from deformation etc?

    plDrawMesh(terrain_mesh);
#endif
}

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
#include "Map.h"
#include "model.h"
#include "game.h"

#include "script/script.h"

#include "client/frontend.h"
#include "client/display.h"

#include "server/actor.h"
#include "script/ScriptConfig.h"
#include "MapManager.h"

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

Map::Map(const GameModeSetup &mode) {
    LogDebug("Loading map, %s, in mode %u\n", mode.map, mode.game_mode);
    /* map manager now worries about resetting map state etc. */

    MapManifest *desc = MapManager::GetInstance()->GetManifest(mode.map);
    if(desc == nullptr) {
        LogWarn("Failed to get map descriptor, %s!\n", mode.map);
    } else {
        LogDebug("Found map descriptor!\n %s\n %s", desc->name.c_str(), desc->description.c_str());
        if(mode.game_mode != MAP_MODE_EDITOR && !(desc->flags & mode.game_mode)) {
            throw std::runtime_error("This mode is unsupported by this map, " + std::string(mode.map) + ", aborting!\n");
        }

        name_           = desc->name;
        description_    = desc->description;
        sky_            = desc->sky;
    }

    std::string base_path = "maps/" + std::string(mode.map) + "/";
    std::string p = u_find(std::string(base_path + std::string(mode.map) + ".pmg").c_str());
    if(!plFileExists(p.c_str())) {
        throw std::runtime_error("PMG, " + p + ", doesn't exist!\n");
    }

    LoadTiles(p);

    p = u_find(std::string(base_path + std::string(mode.map) + ".pog").c_str());
    if (!plFileExists(p.c_str())) {
        throw std::runtime_error("POG, " + p + ", doesn't exist!\n");
    }

    LoadSpawns(p);

    LoadTextures(p);

    sky_model_ = LoadModel("skys/skydome", true);

    GenerateOverview();

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

    if(sky_model_ != nullptr) {
        plDeleteModel(sky_model_);
    }
}

MapChunk *Map::GetChunk(const PLVector2 &pos) {
    uint idx = ((uint)(pos.x) / MAP_CHUNK_PIXEL_WIDTH) + (((uint)(pos.y) / MAP_CHUNK_PIXEL_WIDTH) * MAP_CHUNK_ROW);
    if(idx >= chunks_.size()) {
        LogWarn("attempted to get an out of bounds chunk index!\n");
        return nullptr;
    }

    //LogDebug("chunk idx = %d\n", idx);

    return &chunks_[idx];
}

MapTile *Map::GetTile(const PLVector2 &pos) {
    MapChunk *chunk = GetChunk(pos);
    if(chunk == nullptr) {
        return nullptr;
    }

    uint idx = (((uint)(pos.x) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) +
               ((((uint)(pos.y) / MAP_TILE_PIXEL_WIDTH) % MAP_CHUNK_ROW_TILES) * MAP_CHUNK_ROW_TILES);
    if(idx >= MAP_CHUNK_TILES) {
        LogWarn("attempted to get an out of bounds tile index!\n");
        return nullptr;
    }

    //LogDebug("tile idx = %d\n", idx);
    //LogDebug("tile x = %d ",(((uint)(pos->x) / MAP_TILE_PIXEL_WIDTH)) );
    //LogDebug("tile y = %d\n", (((uint)(pos->y) / MAP_TILE_PIXEL_WIDTH) ));

    return &chunk->tiles[idx];
}

float Map::GetHeight(const PLVector2 &pos) {
    MapTile *tile = GetTile(pos);
    if(tile == nullptr) {
        return 0;
    }

    float tile_x = pos.x - std::floor(pos.x);
    float tile_y = pos.y - std::floor(pos.y);

    float x = tile->height[0] + ((tile->height[1] - tile->height[0]) * tile_x);
    float y = tile->height[2] + ((tile->height[3] - tile->height[2]) * tile_x);
    float z = x + ((y - x) * tile_y);

    return z;
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

void Map::LoadSpawns(const std::string &path) {
    std::ifstream ifs(path, std::ios_base::in | std::ios_base::binary);
    if(!ifs.is_open()) {
        Error("Failed to open actor data, \"%s\", aborting!\n", path.c_str());
    }

    uint16_t num_indices;
    try {
        ifs.read(reinterpret_cast<char *>(&num_indices), sizeof(uint16_t));
    } catch(std::ifstream::failure &err) {
        Error("Failed to read POG indices count, \"%s\", aborting!\n%s (%d)\n", err.what(), err.code().value());
    }

    spawns_.reserve(num_indices);

    try {
        ifs.read(reinterpret_cast<char *>(&spawns_[0]), sizeof(MapSpawn) * num_indices);
    } catch(std::ifstream::failure &err) {
        Error("Failed to read POG spawns, \"%s\", aborting!\n%s (%d)\n", err.what(), err.code().value());
    }

#if 0
    for(unsigned int i = 0; i < num_indices; ++i) {
#define v(a) spawns_[i].a[0], spawns_[i].a[1], spawns_[i].a[2]
        LogDebug("name %s\n", spawns_[i].name);
        LogDebug("position %d %d %d\n", v(position));
        LogDebug("bounds %d %d %d\n", v(bounds));
        LogDebug("angles %d %d %d\n", v(angles));
        LogDebug("fallback %d %d %d\n", v(fallback_position));
    }
#endif

    ifs.close();
}

void Map::LoadTiles(const std::string &path) {
    FILE *fh = std::fopen(path.c_str(), "rb");
    if(fh == nullptr) {
        Error("Failed to open tile data, \"%s\", aborting\n", path.c_str());
    }

    chunks_.resize(MAP_CHUNKS);

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

            struct __attribute__((packed)) {
                int16_t     height{0};
                uint16_t    lighting{0};
            } vertices[25];
            if(std::fread(vertices, sizeof(*vertices), 25, fh) != 25) {
                Error("Unexpected end of file, aborting!\n");
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

                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].type     = (tile.type & 31U);
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].flags    = (tile.type & ~31U);
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].flip     = tile.rotation;
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].slip     = 0;
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].tex      = tile.texture;

                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].height[0] = vertices[
                            (tile_y * 5) + tile_x].height;
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].height[1] = vertices[
                            (tile_y * 5) + tile_x + 1].height;
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].height[2] = vertices[
                            ((tile_y + 1) * 5) + tile_x].height;
                    CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].height[3] = vertices[
                            ((tile_y + 1) * 5) + tile_x + 1].height;

                    u_assert(CUR_CHUNK.tiles[tile_x + tile_y * MAP_CHUNK_ROW_TILES].type < MAX_TILE_TYPES,
                             "invalid tile type!\n");

#if 0
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

void Map::LoadTextures(const std::string &path) {
    static const char *default_sky = "skys/sunny/";
    if(sky_.empty() || sky_ == "none") {
        LogWarn("No sky specified, using default\n");
        sky_ = u_find(default_sky);
    }

    std::string sky_path = u_find("skys/") + sky_ + "1";
    if(!plPathExists(sky_path.c_str())) {
        LogWarn("Failed to find texture path for sky at \"%s\"!\n", sky_path.c_str());
        return;
    }

    /*
    FILE *fh = fopen(path, "rb");
    if(fh == NULL) {
        Error("failed to open texture data, \"%s\", aborting\n", path);
    }

    fclose(fh);
     */
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
            MapTile *tile = GetTile(position);
            if(tile == nullptr) {
                Error("hit an invalid tile during minimap generation!\n");
            }

            auto mod = static_cast<uint8_t>(GetHeight(position) / 100);

            // TODO: some obvious improvements to PLColour can be made here...
            PLColour rgb = (colours[tile->type] / uint8_t(16)) * PLColour(mod, mod, mod);
            if(tile->flags & TILE_FLAG_MINE) {
                rgb = PLColour(255, 0, 0);
            }

            *(buf++) = rgb.r; *(buf++) = rgb.g; *(buf++) = rgb.b;
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

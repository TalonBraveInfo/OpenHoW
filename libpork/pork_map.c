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

#include "pork_engine.h"
#include "pork_map.h"

#include "server/server_actor.h"

typedef struct MapDesc {
    const char *name;
    const char *description;
    unsigned int flags;
} MapDesc;

/* for now these are hard-coded, but
 * eventually we'll do this through a
 * script instead
 */
MapDesc map_descriptors[]={
        // Single-player

        {"camp", "Boot camp", MAP_MODE_SINGLEPLAYER},
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
        {"archi", "You Hillock", MAP_MODE_SURVIVAL_EXPERT | MAP_MODE_DEATHMATCH},
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

MapDesc *GetMapDescription(const char *name) {
    for(unsigned int i = 0; i < plArrayElements(map_descriptors); ++i) {
        if(strcmp(map_descriptors[i].name, name) == 0) {
            return &map_descriptors[i];
        }
    }

    /* todo, dynamically load these in from an external list of sorts
     * this is probably best done by scanning through the maps directory
     * for some sort of extension file that describes each map such as it's
     * name, supported modes and anything else we might want to do in the
     * future.
     *
     * doing it this way will ensure it's easier to package up custom maps
     * in the future :)
     */

    return NULL;
}

////////////////////////////////////////////////////////////////

typedef struct MapTile {

} MapTile;

typedef struct MapBlock {

} MapBlock;

struct {
    char name[24];
    char desc[256];

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
} map_state;

PLMesh *water_mesh = NULL;
PLMesh *terrain_mesh = NULL;

/* possible optimisations...
 *  a) tiles that use the same texture are part of the same mesh instance
 *  b) convert all tiles into a single texture, upload to GPU - using all tiles as single mesh
 *  c) keep all tiles separate, apply simple ray tracing algorithm to check whether tile is visible from camera
 */

void InitMaps(void) {
    terrain_mesh = plCreateMesh(PL_MESH_TRIANGLE_STRIP, PL_DRAW_STATIC, 64, 256);
    if(terrain_mesh == NULL) {
        Error("failed to create terrain mesh, %s, aborting!\n", plGetError());
    }

    water_mesh = plCreateMesh(PL_MESH_TRIANGLE_STRIP, PL_DRAW_STATIC, 16, 64);
    if(water_mesh == NULL) {
        Error("failed to create water mesh, %s, aborting!\n", plGetError());
    }

    plClearMesh(water_mesh);

    // todo, outline water mesh...

    plUploadMesh(water_mesh);
}

void ShutdownMaps(void) {

}

/* unloads the current map from memory
 */
void UnloadMap(void) {
    if(map_state.name[0] == '\0') {
        return;
    }

    pork_free(map_state.spawns);

    memset(&map_state, 0, sizeof(map_state));
}

/* resets the state of the map to how
 * it was when it was first loaded into
 * memory
 */
void ResetMap(void) {
    if(map_state.name[0] == '\0') {
        LogWarn("attempted to reset map, but no map is currently loaded, aborting\n");
        return;
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

    map_state.spawns = calloc(num_indices, sizeof(map_state.spawns));
    if(map_state.spawns == NULL) {
        Error("failed to allocate enough memory for actor spawns, aborting\n");
    }

    memset(map_state.spawns, 0, sizeof(map_state.spawns) * num_indices);

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

    SVClearActors();
}

/* loads a new map into memory - if the config
 * matches that of the currently loaded map
 * then it's ignored
 */
void LoadMap(const char *name, unsigned int mode) {
    if(map_state.name[0] != '\0') {
        if(strncmp(map_state.name, name, sizeof(map_state.name)) == 0) {
            if(map_state.flags == mode) {
                LogWarn("attempted to load duplicate map, \"%s\", aborting\n", name);
                return;
            }

            LogInfo("attempted to load duplicate map, \"%s\", but with alternate state, resetting\n", name);
            ResetMap();
            return;
        }
    }

    MapDesc *desc = GetMapDescription(name);
    if(desc == NULL) {
        LogWarn("failed to get descriptor for map, \"%s\", aborting\n", name);
        return;
    }

    if(!(desc->flags & mode)) {
        LogWarn("the mode you're attempting to use is unsupported by this map, \"%s\", aborting\n", name);
        return;
    }

    UnloadMap();

    char map_path[PL_SYSTEM_MAX_PATH];
    snprintf(map_path, sizeof(map_path), "%s/maps/%s/", g_state.base_path, name);
    if(!plPathExists(map_path)) {
        LogWarn("failed to load map, path \"%s\" doesn't exist, aborting\n", map_path);
        return;
    }

    char pmg_path[PL_SYSTEM_MAX_PATH];
    snprintf(pmg_path, sizeof(pmg_path), "%s/%s.pmg", map_path, name);
    if(!plFileExists(pmg_path)) {
        LogWarn("failed to load pmg, file \"%s\" doesn't exist, aborting\n", pmg_path);
        return;
    }

    if(g_state.is_host) {
        char pog_path[PL_SYSTEM_MAX_PATH];
        snprintf(pog_path, sizeof(pog_path), "%s/%s.pog", map_path, name);
        if (!plFileExists(pog_path)) {
            LogWarn("failed to load pog, file \"%s\" doesn't exist, aborting\n", pog_path);
            return;
        }

        LoadMapSpawns(pog_path);
    }
}

void DrawMap(void) {
    // todo, draw sky, clouds

    plDrawMesh(water_mesh);

    // todo, update parts of terrain mesh from deformation etc?
    plDrawMesh(terrain_mesh);
}
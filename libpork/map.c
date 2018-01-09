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

#include "engine.h"

#define MAP_MODE_SINGLEPLAYER   (1 << 1)
#define MAP_MODE_DEATHMATCH     (1 << 2)

#define MAP_MODE_SURVIVAL_NOVICE    (1 << 3)
#define MAP_MODE_SURVIVAL_EXPERT    (1 << 4)
#define MAP_MODE_SURVIVAL_STRATEGY  (1 << 5)

#define MAP_MODE_GENERATED  (1 << 6)

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
        if(strncmp(name, map_descriptors[i].name, sizeof(map_descriptors[i].name)) == 0) {
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

    unsigned int flags;
} current_map;

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

/* unloads the current map from memory
 */
void UnloadMap(void) {
    if(current_map.name[0] == '\0') {
        return;
    }

    memset(&current_map, 0, sizeof(current_map));
}

/* resets the state of the map to how
 * it was when it was first loaded into
 * memory
 */
void ResetMap(void) {
    if(current_map.name[0] == '\0') {
        LogWarn("attempted to reset map, but no map is currently loaded, aborting\n");
        return;
    }
}

/* loads a new map into memory - if the config
 * matches that of the currently loaded map
 * then it's ignored
 */
void LoadMap(const char *name, unsigned int flags) {
    if(current_map.name[0] != '\0') {
        if(strncmp(current_map.name, name, sizeof(current_map.name)) == 0) {
            if(current_map.flags == flags) {
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

    if(!(desc->flags & flags)) {
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

    char pog_path[PL_SYSTEM_MAX_PATH];
    snprintf(pog_path, sizeof(pog_path), "%s/%s.pog", map_path, name);
    if(!plFileExists(pog_path)) {
        LogWarn("failed to load pog, file \"%s\" doesn't exist, aborting\n", pog_path);
        return;
    }

    char pmg_path[PL_SYSTEM_MAX_PATH];
    snprintf(pmg_path, sizeof(pmg_path), "%s/%s.pmg", map_path, name);
    if(!plFileExists(pmg_path)) {
        LogWarn("failed to load pmg, file \"%s\" doesn't exist, aborting\n", pmg_path);
        return;
    }

}

void DrawMap(void) {
    // todo, draw sky, clouds

    plDrawMesh(water_mesh);

    // todo, update parts of terrain mesh from deformation etc?
    plDrawMesh(terrain_mesh);
}
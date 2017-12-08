/* OpenHOW
 * Copyright (C) 2017-2018 Mark E Sowden
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

/* libpork - base library shared between launcher and editor */

#include <PL/platform_math.h>

#define PORK_TITLE          "OpenHoW"
#define PORK_EDITOR_TITLE   "OpenHoW Editor"
#define PORK_LOG            "pork"
#define PORK_EDITOR_LOG     "pork-editor"
#define PORK_BASE_DIR       "pork"

///////////////////////////////////////////////////

#define print(...)          printf(__VA_ARGS__); plWriteLog(PORK_LOG, __VA_ARGS__)
#define print_error(...)    print(__VA_ARGS__); plWriteLog(PORK_LOG, __VA_ARGS__); exit(-1)
#ifdef _DEBUG
#   define print_debug(...)      print(__VA_ARGS__)
#else
#   define DPRINT(...)      (__VA_ARGS__)
#endif

///////////////////////////////////////////////////

// todo, the below is a basic outline for API

///////////////////////////////////////////////////

#define MAP_WIDTH   2048

enum {
    FT_MUD,
    FT_GRASS,
    FT_METAL,
    FT_WOOD,
    FT_WATER,
    FT_STONE,
    FT_ROCK,
    FT_SAND,
    FT_ICE,
    FT_SNOW,
    FT_QUAG,
    FT_LAVA,
};

#define TILE_FLAG_WATERY    32
#define TILE_FLAG_MINE      64
#define TILE_FLAG_WALL      128

///////////////////////////////////////////////////

#define MAX_PLAYERS 16

///////////////////////////////////////////////////

#define MAX_STATIC_OBJECTS  1024

enum { // Object flags
    OBJECT_FLAG_PLAYABLE    = 1,
    OBJECT_FLAG_SCRIPTED    = 16,
    OBJECT_FLAG_INSIDE      = 32,
    OBJECT_FLAG_DELAYED     = 64,
};

enum { // event types
    OBJECT_EVENT_NONE,                 // does nothing!
    OBJECT_EVENT_ITEM,                 // spawns an item on destruction
    OBJECT_EVENT_PROMOTION,            // spawns a promotion point on destruction
    OBJECT_EVENT_PROTECT = 4,          // spawns a promotion point if the object is not destroyed
    OBJECT_EVENT_AIRDROP_ITEM = 7,     // spawns item airdrop on destruction
    OBJECT_EVENT_AIRDROP_PROMOTION,    // spawns promotion point on destruction
    OBJECT_EVENT_GROUP_ITEM = 13,      // spawns item when group is destroyed
    OBJECT_EVENT_GROUP_PROMOTION,      // spawns promotion point when group is destroyed
    OBJECT_EVENT_REWARD = 19,          // returns specified item on destruction to destructor
    OBJECT_EVENT_GROUP_SPAWN,          // spawns group on destruction
    OBJECT_EVENT_VICTORY,              // triggers victory on destruction
    OBJECT_EVENT_BURST,                // spawns group upon destruction by TNT
    OBJECT_EVENT_GROUP_OBJECT,         // spawns group when object's group is destroyed
};

typedef struct Actor { // Generic Object Properties
    PLVector3 position, angles;
    PLVector3 bounds[2];

    unsigned int spawn_delay;
    unsigned int team;

    int16_t health;

    unsigned int type;
} Actor;

enum {
    ITEM_TROTTER = 1,
    ITEM_KNIFE,
    ITEM_BAYONET,
    ITEM_SWORD,
    ITEM_CATTLEPROD,
    ITEM_PISTOL,
    ITEM_RIFLE,
    ITEM_RIFLE_BURST,
    ITEM_MACHINEGUN,
    ITEM_HMG,
    ITEM_SNIPER_RIFLE,
    ITEM_SHOTGUN,
    ITEM_FLAMETHROWER,
    ITEM_ROCKET_LAUNCHER = 0x0E,
    ITEM_GUIDED_MISSILE = 0x0F,
    ITEM_MEDICINE_DART = 0x10,
    ITEM_TRANQ = 0x11,
    ITEM_GRENADE = 0x12,
    ITEM_CLUSTER_GRENADE = 0x13,
    ITEM_HX_GRENADE = 0x14,
    ITEM_ROLLER_GRENADE = 0x15,
    ITEM_CONFUSION_GAS = 0x16,
    ITEM_FREEZE_GAS = 0x17,
    ITEM_MADNESS_GAS = 0x18,
    ITEM_POISON_GAS = 0x19,
    ITEM_MORTAR = 0x1A,
    ITEM_BAZOOKA = 0x1B,
    ITEM_AIRBURST = 0x1C,
    ITEM_SUPER_AIRBURST = 0x1D,
    ITEM_MEDICINE_BALL = 0x1E,
    ITEM_HOMING_MISSILE = 0x1F,
    ITEM_MINE = 0x20,
    AntiPMine = 0x21,
    ITEM_TNT = 0x22,

    //Vehicle weapons
    VehLongRangeShell = 35,
    VehShockShell,
    Veh1000LBSShell,
    VehFireShell = 0x26,
    VehGasShell = 0x27,
    VehMineShell = 0x28,
    VehHeavyMachineGun = 0x29,
    VehFlamethrower = 0x2A,
    ITEM_VEHICLE_AIRBURST = 0x2B,
    ITEM_VEHICLE_BAZOOKA = 0x2C,
    ITEM_VEHICLE_MORTAR = 0x2D,

    //Missing 0x2E
    //Missing 0x2F
    //Missing 0x30
    //Missing 0x31
    //Missing 0x32

    ITEM_JETPACK = 0x33,
    ITEM_SUICIDE = 0x34,
    ITEM_HEALINGHANDS = 0x35,
    ITEM_SELFHEAL = 0x36,
    ITEM_PICKPOCKET = 0x37,
    ITEM_SHOCKWAVE = 0x38,
    ITEM_SPECIALOPS = 0x39,
    ITEM_AIRSTRIKE = 0x3A,
    FireRainAirstrike = 0x3B,

    //Tools
            ITEM_ENTERVEHICLE = 0x3C,
    ITEM_ENTERBUILDING = 0x3D,
    ITEM_EJECTPIG = 0x3E,
    ITEM_MAPVIEW = 0x3F,
    ITEM_BINOCULARS = 64,
    ITEM_SKIPTURN,
    ITEM_SURRENDER,

    ITEM_SUPERTNT,
    ITEM_HIDE,
    ITEM_SUPERSHOTGUN,
    ITEM_SHRAPNELGRENADE,
    ITEM_GRENADELAUNCHER,

    //Missing 0x48

    ITEM_RANDOM = 73,
    ITEM_HEALTH = 255,

    // Any custom types go here!!!
};
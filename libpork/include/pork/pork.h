/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#pragma once

/* libpork - base library shared between launcher and editor */

#include <PL/platform_log.h>
#include <PL/platform_window.h>
#include <PL/platform_graphics.h>
#include <PL/platform_model.h>
#include <PL/platform_filesystem.h>

#define PORK_TITLE          "OpenHOW"
#define PORK_EDITOR_TITLE   "Crispy Bacon"
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

typedef struct Object { // Generic Object Properties
    PLVector3D position, angles;
    PLVector3D bounds[2];

    unsigned int spawn_delay;
    unsigned int team;

    int16_t health;

    unsigned int type;
} Object;

enum {
    ITEM_TROTTER = 1,
    ITEM_KNIFE,
    ITEM_BAYONET,
    ITEM_SWORD,
    ITEM_CATTLEPROD,
    ITEM_PISTOL,
    ITEM_RIFLE,
    ITEM_RIFLEBURST,
    ITEM_MACHINEGUN,
    ITEM_HMG,
    ITEM_SNIPERRIFLE,
    ITEM_SHOTGUN,
    ITEM_FLAMETHROWER,
    ITEM_ROCKETLAUNCHER = 0x0E,
    ITEM_GUIDEDMISSILE = 0x0F,
    ITEM_MEDICINEDART = 0x10,
    ITEM_TRANQ = 0x11,
    ITEM_GRENADE = 0x12,
    ITEM_CLUSTERGRENADE = 0x13,
    ITEM_HXGRENADE = 0x14,
    RollerGrenade = 0x15,
    ConfusionGas = 0x16,
    FreezeGas = 0x17,
    MadnessGas = 0x18,
    PoisonGas = 0x19,
    Mortar = 0x1A,
    Bazooka = 0x1B,
    Airburst = 0x1C,
    SuperAirburst = 0x1D,
    MedicineBall = 0x1E,
    HomingMissile = 0x1F,
    Mine = 0x20,
    AntiPMine = 0x21,
    TNT = 0x22,

    //Vehicle weapons
    VehLongRangeShell = 35,
    VehShockShell,
    Veh1000LBSShell,
    VehFireShell = 0x26,
    VehGasShell = 0x27,
    VehMineShell = 0x28,
    VehHeavyMachineGun = 0x29,
    VehFlamethrower = 0x2A,
    VehAirburst = 0x2B,
    VehBazooka = 0x2C,
    VehMortar = 0x2D,

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
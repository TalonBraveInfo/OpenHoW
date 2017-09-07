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

#include "main.h"

enum { // Object flags
    OBJECT_FLAG_PLAYABLE    = 1,
    OBJECT_FLAG_SCRIPTED    = 16,
    OBJECT_FLAG_INSIDE      = 32,
    OBJECT_FLAG_DELAYED     = 64,
};

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
    HXGrenade = 0x14,
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
    SpecialOps = 0x39,
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

typedef struct Object { // Generic Object Properties
    PLVector3D  position, angles;
    PLBBox3D    bounds;

    unsigned int spawn_delay;
    unsigned int team;

    unsigned int type;
} Object;

///////////////////////////////////////////////////

typedef struct PlayerObject {
    Object state;

    PLVector3D velocity;

    bool active;

    PLSkeletalModel *model;
} PlayerObject;

typedef struct StaticObject {
    Object state;

    PLStaticModel *model;
} StaticObject;

typedef struct DynamicObject {} DynamicObject;

///////////////////////////////////////////////////

void InitializeObjects(void);
void SpawnObject(
        const char *name,
        PLVector3D position,
        PLVector3D angles
);

void ProcessObjects(void);
void DrawObjects(void);

///////////////////////////////////////////////////
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

#include "object.h"

StaticObject **static_objects;
unsigned int num_static_objects;

///////////////////////////////////////////////////

PlayerObject player_objects[MAX_PLAYERS];
unsigned int num_player_objects;

///////////////////////////////////////////////////

void InitializeObjects(void) {
    PRINT("Initializing objects...\n");

    memset(&player_objects, 0, sizeof(PlayerObject) * MAX_PLAYERS);

    // todo, see how much memory we have available for this?
    //static_objects = (StaticObject**)calloc(4096, sizeof(StaticObject));

    num_player_objects =
    num_static_objects = 0;
}

void ProcessObjects(void) {
   // for(StaticObject *decoration = static_objects[0]; decoration; ++decoration) {

    //}

    for(PlayerObject *player = player_objects; player; ++player) {
        if(!player->active) {
            continue;
        }


    }
}

void DrawObjects(void) {
    //for(StaticObject *decoration = static_objects[0]; decoration; ++decoration) {

   // }

    for(PlayerObject *player = player_objects; player; ++player) {
        if(!player->active) {
            continue;
        }


    }
}

void ClearObjects(void) {
    memset(&player_objects, 0, sizeof(PlayerObject) * MAX_PLAYERS);
}

///////////////////////////////////////////////////

enum {
    OBJECT_TYPE_STATIC,
    OBJECT_TYPE_PLAYER,
    OBJECT_TYPE_DYNAMIC
};

typedef struct ObjectSpawn {
    const char *name;
    const char *model_path;

    unsigned int type;

    void(*Callback)(void);
} ObjectSpawn;

ObjectSpawn spawn_objects[]={
        // Trees
        {"TREEB", NULL},
        {"TREEBH", NULL},
        {"TREEBH1", NULL},
        {"TREEP", NULL},
        {"TREEPA", NULL},
        {"TREEPAH", NULL},
        {"TREEPAH1", NULL},
        {"TREEPAH2", NULL},
        {"TREEPH", NULL},
        {"TREEPH1", NULL},
        {"TREEPH2", NULL},
        {"TREEG", NULL},
        {"TREEGH", NULL},
        {"TREEGH1", NULL},
        {"TREEGH2", NULL},

        {"STUMP1", NULL},

        {"TENT_S", NULL},

        // Bushes
        {"BUSH1", NULL},
        {"BUSH2", NULL},
        {"BUSH2H", NULL},
        {"BUSH3", NULL},

        // Grass
        {"GRASS", NULL},
        {"GRASS1", NULL},
        {"GRASS2", NULL},

        // Flowers
        {"FLOWERS", NULL},
        {"FLOWERS2", NULL},

        // Bridges
        {"BRIDGE_S", NULL},
        {"BRIDGE_C", NULL},
        {"BRIDG_C2", NULL},
        {"BRID2_C", NULL},
        {"BRID2_S2", NULL},

        {"BARBWIRE", NULL},
        {"BARBWIR2", NULL},

        // Crates
        {"CRATE1", NULL, OBJECT_TYPE_DYNAMIC},
        {"CRATE2", NULL, OBJECT_TYPE_DYNAMIC},

        {"FISH", NULL},

        {"STF02PPP", NULL},
        {"STF03PPP", NULL},
        {"STW04_W_", NULL},
        {"STW04_D2", NULL},
        {"STW04PPP", NULL},
        {"STW05_W_", NULL},
        {"STW05PPP", NULL},
        {"STW06_W_", NULL},
        {"STW06PPP", NULL},
        {"STW07PWW", NULL},
        {"S1S_SU02", NULL},
        {"S1S_SU04", NULL},
        {"S1W01PPP", NULL},
        {"W1R06PPP", NULL},
        {"W1W06_W_", NULL},
        {"W1W06DW_", NULL},
        {"W1W07PWW", NULL},
        {"M1S_SU03", NULL},
        {"M1S_WPP", NULL},
        {"M1F01PPP", NULL},
        {"M1W04PPP", NULL},

        {"BIG_GUN", NULL},
        {"DRUM", NULL},
        {"DUMMY", NULL},
        {"SIGN", NULL},
        {"PROPOINT", NULL},
        {"PILLBOX", NULL},
        {"SHELTER", NULL},
        {"MOONS", NULL},
        {"IRONGATE", NULL},
        {"TANK", NULL},
        {"CARRY", NULL},

        {"AMLAUNCH", NULL},
        {"AM_TANK", NULL},

        // Swill Rigs
        {"SWILL2", NULL},
        {"SW2ARM", NULL},

        {"WIND2", NULL},
        {"WIND2V", NULL},
        {"WIND2H", NULL},

        // Pigs
        {"GR_ME", "/british/pcgr_hi", OBJECT_TYPE_PLAYER},
        {"HV_ME", "/british/pchvy_hi", OBJECT_TYPE_PLAYER},
        {"AC_ME", "/british/pcace_hi", OBJECT_TYPE_PLAYER},
        {"SB_ME", "/british/pcsap_hi", OBJECT_TYPE_PLAYER},
        {"LE_ME", "/british/gr_hi", OBJECT_TYPE_PLAYER},
        {"SN_ME", "/british/pcsni_hi", OBJECT_TYPE_PLAYER},
        {"CO_ME", "/british/gr_hi", OBJECT_TYPE_PLAYER},
        {"ME_ME", "/british/pcmed_hi", OBJECT_TYPE_PLAYER},
        {"SP_ME", "/british/pcspy_hi", OBJECT_TYPE_PLAYER},

        {"M_TENT1", NULL},
        {"M_TENT2", NULL},

        {"PIST", NULL},
};

void SpawnObject(const char *name, PLVector3D position, PLVector3D angles) {
    ObjectSpawn *decor = NULL;
    for(unsigned int i = 0; i < plArrayElements(spawn_objects); ++i) {
        if(!strncmp(spawn_objects[i].name, name, 8)) {
            decor = &spawn_objects[i];
            break;
        }
    }

    if(!decor) {
        PRINT("Received an unhandled decoration type, %s, skipping...\n", name);
        return;
    }

    switch(decor->type) {
        case OBJECT_TYPE_PLAYER: {
            player_objects[num_player_objects].active = true;
            player_objects[num_player_objects].position = position;
            num_player_objects++;
            break;
        }

        case OBJECT_TYPE_DYNAMIC: {
            break;
        }

        case OBJECT_TYPE_STATIC: {
            break;
        }

        default: PRINT_ERROR("Invalid object type, %d!\n", decor->type);
    }
}

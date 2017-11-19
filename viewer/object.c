/* OpenHOW
 * Copyright (C) 2017 Mark E Sowden
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
#include "object.h"

///////////////////////////////////////////////////

PlayerObject player_objects[MAX_PLAYERS];
unsigned int num_player_objects;

StaticObject static_objects[MAX_STATIC_OBJECTS];
unsigned int num_static_objects;

void clear_objects(void) {
    memset(&player_objects, 0, sizeof(PlayerObject) * MAX_PLAYERS);
    memset(&static_objects, 0, sizeof(StaticObject) * MAX_STATIC_OBJECTS);

    num_player_objects =
    num_static_objects = 0;
}

///////////////////////////////////////////////////

void init_objects(void) {
    print("Initializing objects...\n");

    clear_objects();
}

void process_objects(void) {
    for(StaticObject *object = static_objects; object < static_objects + num_static_objects; ++object) {
        // todo, stuff
    }

    for(PlayerObject *player = player_objects; player < player_objects + num_player_objects; ++player) {
        if(!player->active) {
            continue;
        }

        // todo, stuff
    }
}

void draw_objects(void) {
    for(StaticObject *object = static_objects; object < static_objects + num_static_objects; ++object) {
        // todo, stuff
    }

    for(PlayerObject *player = player_objects; player < player_objects + num_player_objects; ++player) {
        if(!player->active) {
            continue;
        }

        // todo, stuff
    }
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

        {"FISH", NULL, OBJECT_TYPE_DYNAMIC},

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
        {"SWILL2", "/decor/swill2", OBJECT_TYPE_STATIC},
        {"SW2ARM", "/decor/sw2arm", OBJECT_TYPE_DYNAMIC},

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

void spawn_object(const char *name, PLVector3D position, PLVector3D angles) {
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
            player_objects[num_player_objects].state.position = position;
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

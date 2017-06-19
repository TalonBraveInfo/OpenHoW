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

Object **_objects;
unsigned int num_objects;

void InitializeObjects(void) {
    PRINT("Initializing objects...\n");

    // todo, see how much memory we have available for this?

    num_objects = 0;
    _objects = (Object**)calloc(4096, sizeof(Object));
}

void ProcessObjects(void) {
    for(Object *object = _objects[0]; object; object++) {

    }
}

#define FLAG_ITEM_SPECIALOPS    16
#define FLAG_ITEM_POISONGAS     32

///////////////////////////////////////////////////

typedef struct ObjectSpawn {
    const char *name;
    const char *model_path;

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
        {"CRATE1", NULL},
        {"CRATE2", NULL},

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
        {"GR_ME", NULL},
        {"HV_ME", NULL},
        {"AC_ME", NULL},
        {"SB_ME", NULL},
        {"LE_ME", NULL},
        {"SN_ME", NULL},
        {"CO_ME", NULL},
        {"ME_ME", NULL},
        {"SP_ME", NULL},

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

#if 0
    Object *object = CreateObject();
    object->angles = angles;
    object->position = position;
#endif
}

///////////////////////////////////////////////////

Object *CreateObject(void) {return NULL;}
void DestroyObject(Object *object) {}



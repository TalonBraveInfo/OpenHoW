
#include "pog.h"

// POG Loader

typedef struct SpawnObject {
    const char *name;

    void(*Callback)(void);
} SpawnObject;

SpawnObject spawn_objects[]={
        // Trees
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

        // Bushes
        {"BUSH2", NULL},
        {"BUSH2H", NULL},

        // Grass
        {"GRASS", NULL},
        {"GRASS1", NULL},
        {"GRASS2", NULL},

        // Flowers
        {"FLOWERS", NULL},

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

        {"BIG_GUN", NULL},
        {"DRUM", NULL},
        {"DUMMY", NULL},
        {"SIGN", NULL},
        {"PROPOINT", NULL},
        {"PILLBOX", NULL},
        {"SHELTER", NULL},
        {"MOONS", NULL},
        {"IRONGATE", NULL},

        {"AMLAUNCH", NULL},

        // Swill Rigs
        {"SWILL2", NULL},
        {"SW2ARM", NULL},

        // Pigs
        {"GR_ME", NULL},
        {"HV_ME", NULL},
        {"AC_ME", NULL},
        {"SB_ME", NULL},
        {"LE_ME", NULL},
        {"SN_ME", NULL},
        {"CO_ME", NULL},

        {"M_TENT1", NULL},
        {"M_TENT2", NULL},

        {"PIST", NULL},
};

typedef struct __attribute__((packed)) POGIndex {
    char name[16];
    char unknown0[16]; // this is always "NULL"

    int8_t unknown1[4];
    int8_t unknown2[2];

    int16_t unknown3; // last byte here is always 0

    int8_t unknown4[4];

    int8_t bulk[50];
} POGIndex;

void LoadPOGFile(const char *path) {
    DPRINT("\nOpening %s...\n", path);

    FILE *file = fopen(path, "rb");
    if(!file) {
        PRINT_ERROR("Failed to open %s!\n", path);
    }

    // Skip the header
    fseek(file, 2, SEEK_SET);

    for(unsigned int i = 0; !feof(file); ++i) {
        POGIndex index;
        memset(&index, 0, sizeof(POGIndex));
        if(fread(&index, sizeof(POGIndex), 1, file) != 1) {
            break;
        }

        if(strncmp(index.unknown0, "NULL", 4) != 0) {
            PRINT_ERROR("Unhandled sub-string in %s at index %d, let a programmer know!\n", path, i);
        }

        DPRINT("(%d) %s, %s\n", i, index.name, index.unknown0);
    }

    fclose(file);
}


#include <pork/pork.h>

struct {

} blocks;

struct {

} tiles[MAP_WIDTH * 2];

void init_map_cache(void) {
    memset(tiles, 0, sizeof(tiles) * MAP_WIDTH * 2);
}

///////////////////////////////////////////////////////////

typedef struct __attribute__((packed)) POGIndex { // should be 94 bytes
    char name[16];      // Reflection of model name, typically (and is actually limited to 8 bytes?)
    char unused0[16];   // this is always "NULL"; never used for anything

    uint16_t x;
    uint16_t y;  // height
    uint16_t z;

    int8_t unknown3[10];

    // these seem to scale out from the origin point
    uint16_t bound_x;
    uint16_t bound_y;
    uint16_t bound_z;

    uint16_t unknown4;
    // if this is set to anything other than 0, then the object doesn't show...?
    // It LOOKS like this might be used to determine if an object is destroyable/simulated
    // used for a lot of the block and bridge objects, hrm

    int16_t spawn_delay;

    int8_t unknown6;

    uint8_t team;
    uint16_t type;      // type of item; for crates determines the object inside
    // this actually seems to be a flag; this is going to be fun to figure out...
    uint16_t unknown;   // Seems to also effect the item type...

    uint32_t quantity;

    int8_t unknown8[26];
} POGIndex;

void load_map_pog(const char *name) {
    char buf[24] = {'\0'};
    sprintf(buf, "%s.pog", name);

    FILE *file = fopen(buf, "rb");
    if(file == NULL) {
        print_error("Failed to open %s!\n", buf);
    }

    uint16_t num_indices = 0;
    if(fread(&num_indices, sizeof(uint16_t), 1, file) != 1 || num_indices == 0) {
        print_error("Invalid number of indices within %s!\n", buf);
    }

    print_debug("Index is %d bytes\n", sizeof(POGIndex));
    print_debug("%d indices\n", num_indices);

    for(unsigned int i = 0; i < num_indices; ++i) {
        POGIndex index;
        memset(&index, 0, sizeof(POGIndex));
        if(fread(&index, sizeof(POGIndex), 1, file) != 1) {
            break;
        }

        if(strncmp(index.unused0, "NULL", 4) != 0) {
            print_error("Unhandled sub-string in %s at index %d, let a programmer know!\n", buf, i);
        }

        print_debug(
                "(%d) %s, %s\n" \
               "     position   (%d %d %d)\n" \
               "     bounds     (%d %d %d)\n" \
               "     unknown    (%d)\n" \
               "     type       (%d)\n" \
               "     hrm...     (%d)\n" \
               "     quantity   (%d)\n",

                i,
                index.name,
                index.unused0,
                index.x, index.y, index.z,
                index.bound_x, index.bound_y, index.bound_z,
                index.unknown4,
                index.type,
                index.unknown,
                index.quantity
        );

#if 0
        Object object = {
                plCreateVector3D(index.x, index.y, index.z),
                //plCreateVector3D()
        };
        SpawnObject(
                index.name,
                plCreateVector3D(index.x, index.y, index.z),
                plCreateVector3D(0, 0, 0)
        );
#endif
    }
}

void load_map(const char *name) {
    load_map_pog(name); // load objects
}

///////////////////////////////////////////////////////////

void write_map_pog(void) {} // todo
void write_map(void) {} // todo
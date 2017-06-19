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

#include "pog.h"
#include "object.h"

// POG Loader

typedef struct __attribute__((packed)) POGIndex { // should be 94 bytes
    char name[16];      // Reflection of model name, typically (and is actually limited to 8 bytes?)
    char unused0[16];   // this is always "NULL"; never used for anything

    int8_t unknown0;
    int8_t x;
    int8_t unknown1;
    int8_t y; // height
    int8_t unknown2;
    int8_t z;

    int8_t unknown3[10];

    // these seem to scale out from the origin point
    int16_t bound_x;
    int16_t bound_y;
    int16_t bound_z;

    int16_t unknown4;   // if this is set to anything other than 0, then the object doesn't show...?
                        // It LOOKS like this might be used to determine if an object is destroyable/simulated
                        // used for a lot of the block and bridge objects, hrm
    int16_t unknown5;   // ditto?
    int16_t unknown6;


    uint16_t type;      // type of item; for crates determines the object inside
                        // this actually seems to be a flag; this is going to be fun to figure out...
    uint16_t unknown;

    uint32_t quantity;

    int8_t unknown8[26];
} POGIndex;

void LoadPOGFile(const char *path) {
    DPRINT("\nOpening %s...\n", path);

    FILE *file = fopen(path, "rb");
    if(!file) {
        PRINT_ERROR("Failed to open %s!\n", path);
    }

    uint16_t num_indices = 0;
    fread(&num_indices, sizeof(uint16_t), 1, file);
    if(num_indices == 0) {
        PRINT_ERROR("Invalid number of indices within %s!\n", path);
    }

    DPRINT("Index is %d bytes\n", sizeof(POGIndex));
    DPRINT("%d indices\n", num_indices);

    for(unsigned int i = 0; i < num_indices; ++i) {
        POGIndex index;
        memset(&index, 0, sizeof(POGIndex));
        if(fread(&index, sizeof(POGIndex), 1, file) != 1) {
            break;
        }

        if(strncmp(index.unused0, "NULL", 4) != 0) {
            PRINT_ERROR("Unhandled sub-string in %s at index %d, let a programmer know!\n", path, i);
        }

        DPRINT("(%d) %s, %s\n" \
               "     position   (%d %d %d)\n" \
               "                (%d %d %d)\n" \
               "     bounds     (%d %d %d)\n" \
               "     unknown    (%d)\n" \
               "     type       (%d)\n" \
               "     hrm...     (%d)\n" \
               "     quantity   (%d)\n",
               i,

               index.name,
               index.unused0,
               index.x, index.y, index.z,
               index.unknown0, index.unknown1, index.unknown2,
               index.bound_x, index.bound_y, index.bound_z,
               index.unknown4,
               index.type,
               index.unknown,
               index.quantity
        );

#if 1
        SpawnObject(
                index.name,
                plCreateVector3D(index.x, index.y, index.z),
                plCreateVector3D(0, 0, 0)
        );
#endif
    }

    fclose(file);
}

void WritePOG(const char *path) {
    // todo
}

// other crap, move this somewhere else...

/* PMG
 * Heightmap!
 */

/* PTG
 * Used for storing textures intended for levels.
 *
 * Start of the file appears to be uint32; number of textures in the pack?
 *
 * Each index appears to be the following makeup...
 *  int32 int32 int16(?) int16 int16 int16
 *  seems to be a bunch of data, followed by another description
 *  followed by texture data(?)
 */

typedef struct PTGHeader {
    uint32_t num_textures;
} PTGHeader;

typedef struct PTGIndex {
    int32_t unknown0[3];
    int16_t unknown1[4];

    int8_t data[32]; // always appears to be 32 bytes, interesting...

    int32_t unknown2[2];
    int16_t unknown3[2];
} PTGIndex;

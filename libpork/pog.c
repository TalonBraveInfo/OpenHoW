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

#include <pork/pork.h>

// POG Loader

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

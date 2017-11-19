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

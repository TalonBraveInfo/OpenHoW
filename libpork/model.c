/* OpenHOW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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
#include "engine.h"

/* HIR Format Specification
 * Used to store our piggy bones.
 */
typedef struct __attribute__((packed)) HIRBone {
    uint32_t parent;
    int16_t coords[3];
    int8_t unknown[10];
} HIRBone;

HIRBone bones[MAX_BONES];

// cache the pigs data into memory, since we
// share it between all of them anyway :)
void InitModelCache(void) {
    print("caching pig.hir\n");

    FILE *file = fopen("./" PORK_CHARS_DIR "/pig.hir", "rb");
    if(file == NULL) {
        print_error("failed to load \"./" PORK_CHARS_DIR "/pig.hir\"!\n");
    }

    memset(bones, 0, sizeof(HIRBone) * MAX_BONES);
    unsigned int num_bones = (unsigned int) fread(bones, sizeof(HIRBone), MAX_BONES, file);
    if(num_bones < MAX_BONES) {
        print_error("unexpected number of bones, %d, less than %d!\n", num_bones, MAX_BONES);
    }
    fclose(file);
}
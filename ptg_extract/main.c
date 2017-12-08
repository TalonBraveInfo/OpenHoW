/* OpenHOW
 * Copyright (C) 2017-2018 Mark E Sowden
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

#include <PL/platform_filesystem.h>

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

int main(int argc, char **argv) {
    printf("ptg_extract tool for " PORK_TITLE "\n");

    plInitialize(argc, argv);

    if(argc < 2) {
        printf("Usage...\n   ptg_extract <path> <output>\n");
        return EXIT_FAILURE;
    }

    const char *ptg_path = argv[1];
    const char *out_path = argv[2];
    if(ptg_path[0] == '\0' || out_path[0] == '\0') {
        printf("invalid path provided, aborting!\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(ptg_path, "rb");
    if(file == NULL) {
        printf("failed to open %s!\n", ptg_path);
        return EXIT_FAILURE;
    }

    uint32_t num_textures;
    if(fread(&num_textures, sizeof(uint32_t), 1, file) != 1) {
        printf("invalid PTG file, failed to get number of textures!\n");
        return EXIT_FAILURE;
    }

    printf("%d textures within package...\n", num_textures);

    size_t tim_size = (plGetFileSize(ptg_path) - sizeof(num_textures)) / num_textures;
    printf("expecting each tim to be %zu bytes each!\n", tim_size);

    if(!plCreateDirectory(out_path)) {
        printf("failed to create output directory, %s!\n", out_path);
        return EXIT_FAILURE;
    }

    for(unsigned int i = 0; i < num_textures; ++i) {
        printf("loading %d/%d\n", i + 1, num_textures);

        uint8_t tim[tim_size];
        if(fread(tim, tim_size, 1, file) != 1) {
            printf("failed to read tim!\n");
            return EXIT_FAILURE;
        }

        char destination[PL_SYSTEM_MAX_PATH] = { '\0' };
        sprintf(destination, "%s/%d.tim", out_path, i);
        FILE *out = fopen(destination, "wb");
        if(out == NULL) {
            printf("failed to open %s for writing!\n", destination);
            return EXIT_FAILURE;
        }

        if(fwrite(tim, tim_size, 1, out) != 1) {
            printf("failed to write tim, %s!\n", destination);
            return EXIT_FAILURE;
        }

        fclose(out);
    }

    fclose(file);

    printf("done!\n");
    return EXIT_SUCCESS;
}


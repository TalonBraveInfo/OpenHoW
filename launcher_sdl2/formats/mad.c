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
#include "mad.h"

/*  MAD/MTD Format Specification    */
/* The MAD/MTD format is the package format used by
 * Hogs of War to store and index content used by
 * the game.
 *
 * Files within these packages are expected to be in
 * a specific order, as both the game and other assets
 * within the game rely on this order so that they, for
 * example, will know which textures to load in / use.
 *
 * Because of this, any package that's recreated will need
 * to be done so in a way that preserves the original file
 * order.
 *
 * Thanks to solemnwarning for his help on this one!
 */

void extract_mad_package(const char *path) {
    if(!path || (path[0] == '\0') || (strstr(path, "mcap") != 0) || (strstr(path, "allmad") != 0)) {
        DPRINT("Encountered invalid MAD %s, aborting!\n", path);
        return;
    }

    FILE *file = fopen(path, "rb");
    if(!file) {
        PRINT_ERROR("Failed to load %s!\n", path);
    }

    char package_name[PL_SYSTEM_MAX_PATH] = { '\0' };
    plStripExtension(package_name, plGetFileName(path));
    plLowerCasePath(package_name);

    char package_extension[4] = { '\0' };
    snprintf(package_extension, sizeof(package_extension), "%s", plGetFileExtension(path));
    plLowerCasePath(package_extension);

#if 0
    char index_path[PL_SYSTEM_MAX_PATH] = { 0 };
    snprintf(index_path, sizeof(index_path), "./data/%s_%s.index", package_name, package_extension);
    DPRINT("Opening %s\n", index_path);
    FILE *out_index = fopen(index_path, "w");
    if(!out_index) {
        PRINT_ERROR("Failed to open %s!\n", index_path);
    }
#endif

    typedef struct __attribute__((packed)) MADIndex {
        char file[16];

        uint32_t offset;
        uint32_t length;
    } MADIndex;

    unsigned int lowest_offset = UINT32_MAX;
    unsigned int cur_index = 0;
    long position;
    do {

        MADIndex index; cur_index++;
        if (fread(&index, sizeof(MADIndex), 1, file) != 1) {
            PRINT_ERROR("Invalid index size for %s!\n", path);
        }

        position = ftell(file);
        if (lowest_offset > index.offset) {
            lowest_offset = index.offset;
        }

        const char *ext = plGetFileExtension(index.file);
        if(!ext || ext[0] == '\0') {
            DPRINT("Missing/invalid extension for %s!\n", index.file);
            continue;
        }

        char file_path[PL_SYSTEM_MAX_PATH];
        if(!strncasecmp(ext, "tim", 3) || !strncasecmp(ext, "mgl", 3)) {
            snprintf(file_path, sizeof(file_path), "./data/textures/%s", package_name);
            if(!plCreateDirectory(file_path)) {
                PRINT_ERROR("Failed to create directory at %s!\n");
            }
            snprintf(file_path, sizeof(file_path), "./data/textures/%s/%s", package_name, index.file);
        } else { // we'll assume it's a model
            snprintf(file_path, sizeof(file_path), "./data/models/%s", package_name);
            if(!plCreateDirectory(file_path)) {
                PRINT_ERROR("Failed to create directory at %s!\n");
            }
            snprintf(file_path, sizeof(file_path), "./data/models/%s/%s", package_name, index.file);
        }

        plLowerCasePath(file_path);

#if 0
        fprintf(out_index, "%d %s (%s)\n", cur_index, file_path, index.file);
#endif

        fseek(file, index.offset, SEEK_SET);
        uint8_t *data = calloc(index.length, sizeof(uint8_t));
        if (fread(data, sizeof(uint8_t), index.length, file) == index.length) {
            DPRINT("Writing %s...\n", file_path);

            FILE *out = fopen(file_path, "wb");
            if (!out || fwrite(data, sizeof(uint8_t), index.length, out) != index.length) {
                PRINT_ERROR("Failed to write %s!\n", file_path);
            }
            fclose(out);
        }
        free(data);

        fseek(file, position, SEEK_SET);
    } while(position < lowest_offset);

    fclose(file);
#if 0
    fclose(out_index);
#endif
}

void InitializeMADPackages(void) {

    PRINT("\nExtracting MAD/MTD packages...\n");

    if (g_state.is_psx) { // tidy file paths
        plScanDirectory("./tims/", ".mad", ExtractMADPackage, false);
        plScanDirectory("./fe/", ".mad", ExtractMADPackage, false);
        plScanDirectory("./chars/", ".mad", ExtractMADPackage, false);
    } else { // sloppy file paths
        plScanDirectory("./Chars/", ".mad", ExtractMADPackage, false);
        plScanDirectory("./Chars/", ".mtd", ExtractMADPackage, false);
        plScanDirectory("./Maps/", ".mad", ExtractMADPackage, false);
        plScanDirectory("./Maps/", ".mtd", ExtractMADPackage, false);
    }

    PRINT("Done!\n");
}
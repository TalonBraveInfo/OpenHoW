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

typedef struct __attribute__((packed)) MADIndex {
    char file[12];

    int32_t padding0;

    uint32_t offset;
    uint32_t length;
} MADIndex;

//////////////////////////////////////////////////////////

void ExtractMADPackage(const char *path) {
    if(!path || (path[0] == '\0') || (strstr(path, "mcap") != 0) || (strstr(path, "allmad") != 0)) {
        DPRINT("Encountered invalid MAD %s, aborting!\n", path);
        return;
    }

    FILE *file = fopen(path, "rb");
    if(!file) {
        PRINT_ERROR("Failed to load %s!\n", path);
    }

    char package_name[PL_SYSTEM_MAX_PATH] = { 0 };
    plStripExtension(package_name, plGetFileName(path));
    plLowerCasePath(package_name);

    char package_extension[4] = { 0 };
    snprintf(package_extension, sizeof(package_extension), "%s", plGetFileExtension(path));
    plLowerCasePath(package_extension);

    char index_path[PL_SYSTEM_MAX_PATH] = { 0 };
    snprintf(index_path, sizeof(index_path), "./data/%s_%s.index", package_name, package_extension);
    DPRINT("Opening %s\n", index_path);
    FILE *out_index = fopen(index_path, "w");
    if(!out_index) {
        PRINT_ERROR("Failed to open %s!\n", index_path);
    }

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

        fprintf(out_index, "%d %s (%s)\n", cur_index, file_path, index.file);

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

        fseek(file, position, SEEK_SET);
    } while(position < lowest_offset);

    fclose(file); fclose(out_index);
}

void InitializeMADPackages(void) {

    PRINT("\nExtracting MAD/MTD packages...\n");

    if (g_state.is_psx) { // tidy file paths
        plScanDirectory("./tims/", ".mad", ExtractMADPackage);
        plScanDirectory("./fe/", ".mad", ExtractMADPackage);
        plScanDirectory("./chars/", ".mad", ExtractMADPackage);
    } else { // sloppy file paths
        plScanDirectory("./Chars/", ".mad", ExtractMADPackage);
        plScanDirectory("./Chars/", ".mtd", ExtractMADPackage);
        plScanDirectory("./Maps/", ".mad", ExtractMADPackage);
        plScanDirectory("./Maps/", ".mtd", ExtractMADPackage);
    }

    PRINT("Done!\n");
}
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
    const char *name = plGetFileName(path);
    if(!name || (name[0] == '\0') ||
       // Known MAD packages that aren't valid
       (strncmp(name, "mcap.mad", 8) == 0) ||      // animations
       (strncmp(name, "mcapx.mad", 9) == 0) ||     // looks like some executable code? (psx only)
       (strncmp(name, "femcap.mad", 10) == 0) ||   // more animations? (psx only)
       (strncmp(name, "allmad.mad", 10) == 0)      // MORE animations!? (psx only)
            ) {
        return;
    }

    PRINT("...");

    FILE *file = fopen(path, "rb");
    if(!file) {
        PRINT_ERROR("Failed to load file %s!\n", path);
    }

    char outpath[PL_SYSTEM_MAX_PATH] = { 0 };
    plStripExtension(outpath, path);
    snprintf(outpath, sizeof(outpath), "./%s", outpath);
    if(!plCreateDirectory(outpath)) {
        PRINT_ERROR("Failed to create directory!\n%s\n", plGetError());
    }

    char foutfile[PL_SYSTEM_MAX_PATH];
    snprintf(foutfile, sizeof(foutfile), "%s.index", outpath);
    FILE *iout = fopen(foutfile, "w");
    if(!iout) {
        PRINT_ERROR("Failed to open index!\n");
    }

    unsigned int lowest_offset = UINT32_MAX; fpos_t position; unsigned int cur_index = 0;
    do {

        MADIndex index;
        if(fread(&index, sizeof(MADIndex), 1, file) != 1) {
            PRINT_ERROR("Invalid index size! (%s)\n", path);
        }

        fgetpos(file, &position);

        fprintf(iout, "%d", cur_index); cur_index++;

        if(lowest_offset > index.offset) {
            lowest_offset = index.offset;
        } else if(index.padding0 != 0) {
            continue;
        }

        fprintf(iout, " %s\n", index.file);

        char foutpath[PL_SYSTEM_MAX_PATH];
        snprintf(foutpath, sizeof(foutpath), "%s/%s", outpath, index.file);
        if(plFileExists(foutpath)) {
            continue;
        }

        fseek(file, index.offset, SEEK_SET);
        uint8_t *data = calloc(index.length, sizeof(uint8_t));
        if(fread(data, sizeof(uint8_t), index.length, file) == index.length) {
            FILE *out = fopen(foutpath, "wb");
            if(!out || fwrite(data, sizeof(uint8_t), index.length, out) != index.length) {
                PRINT("Failed!\n");
            }
            fclose(out);
        }

        fsetpos(file, &position);

    } while(position < lowest_offset);

    fclose(file); fclose(iout);
}

void InitializeMADPackages(void) {

    PRINT("\nExtracting MAD/MTD packages...");

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
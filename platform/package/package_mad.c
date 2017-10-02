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

#include "package_private.h"

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
    char file[16];

    uint32_t offset;
    uint32_t length;
} MADIndex;

PLPackage *_plLoadMADPackage(const char *filename, bool precache) {
    FILE *fh = NULL;
    PLPackage *package = NULL;

    fh = fopen(filename, "rb");
    if(fh == NULL) {
        goto FAILED;
    }

    size_t file_size = plGetFileSize(filename);
    if(plGetFunctionResult() != PL_RESULT_SUCCESS) {
        goto FAILED;
    }

    /* Figure out the number of headers in the MAD file by reading them in until we cross into the data region of one
     * we've previously loaded. Checks each header is valid.
     */

    size_t data_begin = file_size;
    unsigned int num_indices = 0;

    while((num_indices + 1) * sizeof(MADIndex) <= data_begin) {
        MADIndex index;
        if(fread(&index, sizeof(MADIndex), 1, fh) != 1) {
            /* EOF, or read error */
            goto FAILED;
        }

        // ensure the file name is valid...
        for(unsigned int i = 0; i < 16; ++i) {
            if(isprint(index.file[i]) == 0 && index.file[i] != '\0') {
                goto FAILED;
            }
        }

        if(index.offset >= file_size || (uint64_t)(index.offset) + (uint64_t)(index.length) > file_size) {
            /* File offset/length falls beyond end of file */
            goto FAILED;
        }

        if(index.offset < data_begin)
        {
            data_begin = index.offset;
        }

        ++num_indices;
    }

    /* Allocate the basic package structure now we know how many files are in the archive. */

    package = malloc(sizeof(PLPackage));
    if(package == NULL) {
        goto FAILED;
    }

    memset(package, 0, sizeof(package));

    package->path = malloc(strlen(filename) + 1);
    if(package->path == NULL) {
        goto FAILED;
    }

    strcpy(package->path, filename);

    package->table_size = num_indices;
    package->table      = calloc(num_indices, sizeof(struct PLPackageIndex));
    if(package->table == NULL) {
        goto FAILED;
    }

    /* Rewind the file handle and populate package->table with the metadata from the headers. */

    rewind(fh);

    for(unsigned int i = 0; i < num_indices; ++i) {
        MADIndex index;
        if(fread(&index, sizeof(MADIndex), 1, fh) != 1) {
            /* EOF, or read error */
            goto FAILED;
        }

        strncpy(package->table[i].name, index.file, sizeof(index.file));
        package->table[i].name[sizeof(index.file)] = '\0';

        package->table[i].length = index.length;
        package->table[i].offset = index.offset;
    }

    /* Read in each file's data */

    if(precache) {
        for (unsigned int i = 0; i < num_indices; ++i) {
            _plLoadMADPackageFile(fh, &(package->table[i]));
        }
    }

    fclose(fh);

    return package;

    FAILED:

    if(package != NULL) {
        for(unsigned int i = 0; i < package->table_size; ++i) {
            free(package->table[i].data);
        }

        free(package->table);
        free(package->path);
        free(package);
    }

    if(fh != NULL) {
        fclose(fh);
    }

    return NULL;
}

bool _plLoadMADPackageFile(FILE *fh, PLPackageIndex *pi) {
    pi->data = malloc(pi->length);
    if(pi->data == NULL) {
        return false;
    }

    if(fseek(fh, pi->offset, SEEK_SET) != 0 || fread(pi->data, pi->length, 1, fh) != 1) {
        free(pi->data);
        pi->data = NULL;

        return false;
    }

    return true;
}

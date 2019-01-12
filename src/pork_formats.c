/* OpenHoW
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

#include <PL/platform_package.h>

#include "pork_engine.h"
#include "pork_formats.h"
#include "pork_particle.h"

/* Hogs of War file formats
 * -----------------------------------
 * BIN : Audio/Model/Texture data
 * DAT : Audio data
 * MAM : Mangled texture / model package
 * MAD : Texture / model package
 * MTM : Mangled model texture package
 * MTD : Texture / model package
 * MMM : Mangled model package
 * MGL : Mangled texture data
 * FAC : Model faces
 * VTX : Model vertices
 * NO2 : Model normals
 * HIR : Model skeleton
 * POM : Mangled map object data
 * POG : Map object data
 * PTM : Mangled map textures package
 * PTG : Map Textures package
 * OFF : File offset sizes
 */

//const char *supported_model_formats[]={"vtx", NULL};
const char *supported_image_formats[]={"png", "tga", "bmp", "tim", NULL};
//const char *supported_audio_formats[]={"wav", NULL};
//const char *supported_video_formats[]={"bik", NULL};

/******************************************************************/
/* MAD/MTD Format Specification */

typedef struct __attribute__((packed)) MADIndex {
    char file[16];
    uint32_t offset;
    uint32_t length;
} MADIndex;

static bool LoadMADPackageFile(FILE *fh, PLPackageIndex *pi) {
    pi->file.data = pl_malloc(pi->file.size);
    if(pi->file.data == NULL) {
        LogWarn("Failed to allocate %d bytes!\n", pi->file.size);
        return false;
    }

    if(fseek(fh, pi->offset, SEEK_SET) != 0 || fread(pi->file.data, pi->file.size, 1, fh) != 1) {
        free(pi->file.data);
        pi->file.data = NULL;
        return false;
    }

    return true;
}

static PLPackage *LoadMADPackage(const char *path, bool cache) {
    FILE *fh = fopen(path, "rb");
    if(fh == NULL) {
        return NULL;
    }

    PLPackage *package = NULL;

    size_t file_size = plGetFileSize(path);
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

    package = pl_malloc(sizeof(PLPackage));
    if(package == NULL) {
        goto FAILED;
    }

    memset(package, 0, sizeof(PLPackage));

#if 0 // done after package load now
    package->path = pl_malloc(strlen(filename) + 1);
    if(package->path == NULL) {
        goto FAILED;
    }

    strcpy(package->path, filename);
#endif
    package->internal.LoadFile  = LoadMADPackageFile;
    package->table_size         = num_indices;
    package->table              = pl_calloc(num_indices, sizeof(struct PLPackageIndex));
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

        strncpy(package->table[i].file.name, index.file, sizeof(package->table[i].file.name));
        package->table[i].file.name[sizeof(index.file) - 1] = '\0';
        package->table[i].file.size = index.length;
        package->table[i].offset = index.offset;
    }

    /* Read in each file's data */

    if(cache) {
        for (unsigned int i = 0; i < num_indices; ++i) {
            LoadMADPackageFile(fh, &(package->table[i]));
        }
    }

    fclose(fh);

    return package;

    FAILED:

    if(package != NULL) {
        plDeletePackage(package);
    }

    if(fh != NULL) {
        fclose(fh);
    }

    return NULL;
}

/******************************************************************/

void RegisterFormatInterfaces(void) {
    plRegisterPackageInterface("mad", LoadMADPackage);
    plRegisterPackageInterface("mtd", LoadMADPackage);
}
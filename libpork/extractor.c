/* OpenHOW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
 * Copyright (C) 2017 Daniel Collins <solemnwarning@solemnwarning.net>
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

/* Extraction process for initial setup */

bool is_psx = false;

void ExtractPTGPackage(const char *path) {
    if(path[0] == '\0') { // technically, this should never, ever, ever, ever happen...
        printf("encountered invalid path for PTG, aborting!\n");
        return;
    }

    char ptg_name[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(ptg_name, plGetFileName(path));
    pl_strtolower(ptg_name);

    char output_dir[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(output_dir, "./" PORK_TEXTURES_DIR "/maps/%s", ptg_name);
    if(!plCreatePath(output_dir)) {
        printf("failed to create path %s, aborting!\n", output_dir);
        return;
    }

    FILE *file = fopen(path, "rb");
    if(file == NULL) {
        printf("failed to load %s, aborting!\n", path);
        return;
    }

    uint32_t num_textures;
    if(fread(&num_textures, sizeof(uint32_t), 1, file) != 1) {
        printf("invalid PTG file, failed to get number of textures!\n");
        goto TIDY;
    }

    char index_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    snprintf(index_path, sizeof(index_path), "./" PORK_BASE_DIR "/%s_ptg.index", ptg_name);
    FILE *out_index = fopen(index_path, "w");
    if(out_index == NULL) {
        printf("failed to open %s for writing, aborting!\n", index_path);
        goto TIDY;
    }
    fprintf(out_index, "!!GENERATED INDEX FILE, DO NOT MODIFY!!\n");

    FILE *out = NULL;
    size_t tim_size = (plGetFileSize(path) - sizeof(num_textures)) / num_textures;
    for(unsigned int i = 0; i < num_textures; ++i) {
        uint8_t tim[tim_size];
        if(fread(tim, tim_size, 1, file) != 1) {
            printf("failed to read tim, aborting!\n");
            goto TIDY;
        }

        char destination[PL_SYSTEM_MAX_PATH] = {'\0'};
        sprintf(destination, "%s/%d.tim", output_dir, i);
        out = fopen(destination, "wb");
        if(out == NULL) {
            printf("failed to open %s for writing, aborting!\n", destination);
            goto TIDY;
        }

        printf(" %s\n", destination);

        fprintf(out_index, "%d %d %s\n", i, i, destination);

        if(fwrite(tim, tim_size, 1, out) != 1) {
            printf("failed to write %s, aborting!\n", destination);
            goto TIDY;
        }

        pork_fclose(out);
    }

    TIDY:
    pork_fclose(out_index);
    pork_fclose(out);
    pork_fclose(file);
}

void ExtractMADPackage(const char *path) {
    if(path[0] == '\0') { // technically, this should never, ever, ever, ever happen...
        printf("encountered invalid path for MAD, aborting!\n");
        return;
    }

    char package_name[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(package_name, plGetFileName(path));
    pl_strtolower(package_name);

    char package_extension[4] = {'\0'};
    snprintf(package_extension, sizeof(package_extension), "%s", plGetFileExtension(path));
    pl_strtolower(package_extension);

    if(     strcmp(package_name, "mcap") == 0 ||
            strcmp(package_name, "allmad") == 0 ||
            strcmp(package_name, "febmp") == 0) { // special cases...
        return;
    }

    FILE *file = fopen(path, "rb");
    if(file == NULL) {
        printf("failed to load %s, aborting!\n", path);
        return;
    }

    char index_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    snprintf(index_path, sizeof(index_path), "./" PORK_BASE_DIR "/%s_%s.index", package_name, package_extension);
    FILE *out_index = fopen(index_path, "w");
    if(out_index == NULL) {
        printf("failed to open %s for writing, aborting!\n", index_path);
        goto TIDY;
    }
    fprintf(out_index, "!!GENERATED INDEX FILE, DO NOT MODIFY!!\n");

    typedef struct __attribute__((packed)) MADIndex {
        char file[16];

        uint32_t offset;
        uint32_t length;
    } MADIndex;

    FILE *out = NULL;
    uint8_t *data = NULL;
    unsigned int lowest_offset = UINT32_MAX;
    unsigned int cur_index = 0;
    long position;
    do {
        MADIndex index; cur_index++;
        if(fread(&index, sizeof(MADIndex), 1, file) != 1) {
            printf("invalid index size for %s, aborting!\n", package_name);
            goto TIDY;
        }

        position = ftell(file);
        if(lowest_offset > index.offset) {
            lowest_offset = index.offset;
        }

        // this is where the fun begins...

        char file_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        if(pl_strcasestr(path, "maps") != NULL) {
            if(strcmp(package_extension, "mtd") == 0) { // textures
                sprintf(file_path, "./" PORK_TEXTURES_DIR "/models/maps/%s", package_name);
            } else { // models
                sprintf(file_path, "./" PORK_MODELS_DIR "/maps/%s", package_name);
            }
        } else {
            if(strcmp(package_extension, "mtd") == 0) { // textures
                sprintf(file_path, "./" PORK_TEXTURES_DIR "/models/%s", package_name);
            } else { // models
                sprintf(file_path, "./" PORK_MODELS_DIR "/%s", package_name);
            }
        }

        if(!plCreatePath(file_path)) {
            printf("failed to create path %s, aborting!\n", file_path);
            goto TIDY;
        }

        pl_strtolower(index.file);
        strcat(file_path, "/");
        strcat(file_path, index.file);

        fprintf(out_index, "%d %s %s\n", cur_index, index.file, file_path);

        // go and grab the so we can export!
        fseek(file, index.offset, SEEK_SET);

        data = calloc(index.length, sizeof(uint8_t));
        if(data == NULL) {
            printf("failed to allocate %d bytes for export of %s, aborting!\n", index.length, index.file);
            goto TIDY;
        }

        if(fread(data, sizeof(uint8_t), index.length, file) != index.length) {
            printf("failed to read %s in %s, aborting!\n", index.file, package_name);
            goto TIDY;
        }

        out = fopen(file_path, "wb");
        if(out == NULL) {
            printf("failed to open %s for writing, aborting!\n", file_path);
            goto TIDY;
        }

        printf(" %s\n", file_path);

        if(fwrite(data, sizeof(uint8_t), index.length, out) != index.length) {
            printf("failed to write %s!\n", file_path);
            goto TIDY;
        }

        pork_fclose(out);
        pork_free(data);

        // return us to where we were in the file
        fseek(file, position, SEEK_SET);
    } while(position < lowest_offset);

    TIDY:
    pork_free(data);

    pork_fclose(out_index);
    pork_fclose(out);

    pork_fclose(file);
}

bool ExtractGameData(const char *path) {
    if(plPathExists("./" PORK_BASE_DIR)) {
        printf("please delete your ./" PORK_BASE_DIR " if you want to begin extraction again!\n");
        return true;
    }

    printf("unable to find data directory\nextracting game contents from %s...\n", path);

    // Check if it's the PSX or PC version.
    char system_cnf_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(system_cnf_path, "%s/system.cnf", path);
    if(plFileExists(system_cnf_path)) {
        printf("found system.cnf, assuming psx version...\n");
        is_psx = true;
        // todo, continue here? currently unsupported!
    }

    if(!plCreateDirectory("./" PORK_BASE_DIR)) {
        printf("failed to create base directory for data!\n");
        return false;
    }

    printf("extracting MAD/MTD packages...\n");

    plScanDirectory(path, "mad", ExtractMADPackage, true);
    plScanDirectory(path, "mtd", ExtractMADPackage, true);

    printf("extracting PTG packages...\n");

    plScanDirectory(path, "ptg", ExtractPTGPackage, true);

    printf("\nextraction complete!\n");

    return true;
}
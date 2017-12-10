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
#include <PL/platform_image.h>

#include <netinet/in.h>

/* Extraction process for initial setup */

/* Hogs of War directory structure...
 *     Audio
 *     Chars
 *     FEBmps
 *     FESounds
 *     FEText
 *     Maps
 *     Skys
 *     Speech
 */

bool is_psx = false;

char current_target[PL_SYSTEM_MAX_PATH] = {'\0'};

void ExtractPTGPackage(const char *path) {
    if(path == NULL || path[0] == '\0') { // technically, this should never, ever, ever, ever happen...
        printf("encountered invalid path for PTG, aborting!\n");
        return;
    }

    char ptg_name[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(ptg_name, plGetFileName(path));
    pl_strtolower(ptg_name);

    char output_dir[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(output_dir, "%s/%s", current_target, ptg_name);
    if(!plCreatePath(output_dir)) {
        printf("failed to create path %s, aborting!\n", output_dir);
        return;
    }

    FILE *file = fopen(path, "rb");
    if(file == NULL) {
        printf("failed to load %s, aborting!\n", path);
        return;
    }

    FILE *out = NULL;

    uint32_t num_textures;
    if(fread(&num_textures, sizeof(uint32_t), 1, file) != 1) {
        printf("invalid PTG file, failed to get number of textures!\n");
        goto ABORT_PTG;
    }

    size_t tim_size = (plGetFileSize(path) - sizeof(num_textures)) / num_textures;
    for(unsigned int i = 0; i < num_textures; ++i) {
        uint8_t tim[tim_size];
        if(fread(tim, tim_size, 1, file) != 1) {
            printf("failed to read tim, aborting!\n");
            goto ABORT_PTG;
        }

        char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        sprintf(out_path, "%s/%d.tim", output_dir, i);
        out = fopen(out_path, "wb");
        if(out == NULL) {
            printf("failed to open %s for writing, aborting!\n", out_path);
            goto ABORT_PTG;
        }

        printf(" %s\n", out_path);

        if(fwrite(tim, tim_size, 1, out) != 1) {
            printf("failed to write %s, aborting!\n", out_path);
            goto ABORT_PTG;
        }

        pork_fclose(out);
    }

    ABORT_PTG:
    pork_fclose(out);
    pork_fclose(file);
}

void ExtractMADPackage(const char *path) {
    if(path == NULL || path[0] == '\0') { // technically, this should never, ever, ever, ever happen...
        printf("encountered invalid path for MAD, aborting!\n");
        return;
    }

    char package_name[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(package_name, plGetFileName(path));
    pl_strtolower(package_name);

    // certain packages in how are special cases, or
    // completely unnecessary altogether - we'll either
    // ignore these or will be dealing with them later
    if(strcmp(package_name, "mcap") == 0 ||     // Used for storing animation data
       strcmp(package_name, "allmad") == 0 ||
       strcmp(package_name, "white") == 0 ||
       strcmp(package_name, "febmp") == 0) {    // FEBMP isn't necessary, because the MGL files are already outside!
        printf("skipping %s, we'll deal with this later!\n", package_name);
        return;
    }

    FILE *file = fopen(path, "rb");
    if(file == NULL) {
        printf("failed to load %s, aborting!\n", path);
        return;
    }

    char package_extension[4] = {'\0'};
    snprintf(package_extension, sizeof(package_extension), "%s", plGetFileExtension(path));
    pl_strtolower(package_extension);

    FILE *out_index = NULL;
    // check if it's necessary for us to produce an index file
    // since models expect each texture to have a particular
    // position within the MTD package - yay...
    if(strcmp(package_extension, "mtd") == 0) {
        char index_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        snprintf(index_path, sizeof(index_path), "%s/%s/mtd_index", current_target, package_name);
        out_index = fopen(index_path, "w");
        if (out_index == NULL) {
            printf("failed to open %s for writing!\n", index_path);
        } else {
            fprintf(out_index, "!!GENERATED INDEX FILE, DO NOT MODIFY!!\n");
        }
    }

    typedef struct __attribute__((packed)) MADIndex {
        char file[16];

        uint32_t offset;
        uint32_t length;
    } MADIndex;

    uint8_t *data = NULL;
    FILE *out = NULL;
    unsigned int lowest_offset = UINT32_MAX;
    unsigned int cur_index = 0;
    long position;
    do {
        MADIndex index; cur_index++;
        if(fread(&index, sizeof(MADIndex), 1, file) != 1) {
            printf("invalid index size for %s, aborting!\n", package_name);
            goto ABORT_MAD;
        }

        position = ftell(file);
        if(lowest_offset > index.offset) {
            lowest_offset = index.offset;
        }

        // this is where the fun begins...

        // create the path we're going to extract the file to
        char file_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        sprintf(file_path, "%s/%s", current_target, package_name);
        if(!plCreatePath(file_path)) {
            printf("failed to create path %s, aborting!\n", file_path);
            continue;
        }

        // now append that with the file name
        pl_strtolower(index.file);
        strcat(file_path, "/");
        strcat(file_path, index.file);

        // then check if we need to amend our index file
        if(out_index != NULL) {
            // remove the extension, since we're going to support
            // multiple formats for texture loading, and it's
            // unnecessary to read it in later when we're determining
            // what texture to load :)
            char index_file_path[PL_SYSTEM_MAX_PATH] = {'\0'};
            plStripExtension(index_file_path, index.file);
            fprintf(out_index, "%d %s\n", cur_index, index_file_path);
        }

        CHECK_AGAIN:
        // check if we're throwing out any duplicates when copying
        // everything over. technically this shouldn't occur but
        // is primarily being left here for now just for debugging
        // purposes - eventually once we're settled with both the
        // PC and PSX content, we can probably remove it
        if(plFileExists(file_path)) {
            size_t size = plGetFileSize(file_path);
            if(size == index.length) {
                printf("duplicate file found for %s at %s, skipping!\n", index.file, file_path);
                continue;
            }

            // this part should never happen, but we'll check for it anyway, call me paranoid!
            printf("duplicate file found for %s at %s with differing size (%d vs %zu), renaming!\n",
                   index.file, file_path, index.length, size);
            strcat(file_path, "_");
            goto CHECK_AGAIN;
        }

        data = calloc(index.length, sizeof(uint8_t));
        if(data == NULL) {
            printf("failed to allocate %d bytes for export of %s, aborting!\n", index.length, index.file);
            goto ABORT_MAD;
        }

        // go and grab the data so we can export!
        fseek(file, index.offset, SEEK_SET);
        if(fread(data, sizeof(uint8_t), index.length, file) != index.length) {
            printf("failed to read %s in %s, aborting!\n", index.file, package_name);
            goto ABORT_MAD;
        }

        out = fopen(file_path, "wb");
        if(out == NULL) {
            printf("failed to open %s for writing, aborting!\n", file_path);
            goto ABORT_MAD;
        }

        printf(" %s\n", file_path);
        if(fwrite(data, sizeof(uint8_t), index.length, out) != index.length) {
            printf("failed to write %s!\n", file_path);
            goto ABORT_MAD;
        }

        pork_fclose(out);
        pork_free(data);

        // return us to where we were in the file
        fseek(file, position, SEEK_SET);
    } while(position < lowest_offset);

    ABORT_MAD:
    pork_free(data);

    pork_fclose(out_index);
    pork_fclose(out);
    pork_fclose(file);
}

void CopyTextDirectory(const char *path) {
    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(out_path, "./" PORK_FETEXT_DIR "/%s", plGetFileName(path));
    pl_strtolower(out_path);
    printf(" %s\n", out_path);
    if(!plCopyFile(path, out_path)) {
        printf("%s\n", plGetResultString(plGetFunctionResult()));
    }
}

void CopyAudioDirectory(const char *path) {
    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(out_path, "./" PORK_AUDIO_DIR "/%s", plGetFileName(path));
    pl_strtolower(out_path);
    printf(" %s\n", out_path);
    if(!plCopyFile(path, out_path)) {
        printf("%s\n", plGetResultString(plGetFunctionResult()));
    }
}

void CopyMapsDirectory(const char *path) {
    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(out_path, "./" PORK_MAPS_DIR "/%s", plGetFileName(path));
    pl_strtolower(out_path);
    printf(" %s\n", out_path);
    if(!plCopyFile(path, out_path)) {
        printf("%s\n", plGetResultString(plGetFunctionResult()));
    }
}

#include <IL/il.h>
#include <IL/ilu.h>

void ConvertTIMtoPNG(const char *path) {
    // figure out if the file already exists before
    // we even start trying to convert this thing
    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(out_path, path);
    strcat(out_path, ".png");
    if(plFileExists(out_path)) {
        printf("file already exists at \"%s\", skipping!\n", out_path);
        return;
    }

    PLImage image;
    PLresult result = (PLresult)plLoadImage(path, &image);
    if(result != PL_RESULT_SUCCESS) {
        printf("failed to load TIM, \"%s\", %s, aborting!\n", path, plGetError());
        return;
    }

    // ensure that it's a format we're able to convert from
    if(image.format != PL_IMAGEFORMAT_RGB5A1) {
        plFreeImage(&image);
        printf("unexpected pixel format in \"%s\", aborting!\n", path);
        return;
    }

    // whipped up by solemnwarning, thanks! converts the pixel format to RGBA8
    unsigned char *data = malloc(image.width * image.height * 4);
    unsigned char *odata = data;
    uint16_t *idata = (uint16_t*)(image.data[0]);
    for(unsigned int i = 0; i < (image.width * image.height); ++i) {
        uint16_t p = ntohs(*(idata++));
        *(odata++) = (p & 0x001F) << 3;
        *(odata++) = (p & 0x03E0) >> 2;
        *(odata++) = (p & 0x7C00) >> 7;
        *(odata++) = (p & 0x8000) ? 0x00 : 0xFF;
    }

    printf(" %s\n", out_path);

    // todo, eventually this should run through the platform lib

    ILuint image_id = ilGenImage();
    ilBindImage(image_id);
    ilTexImage(image.width, image.height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, data);
    iluFlipImage();

    ilEnable(IL_FILE_OVERWRITE);
    ilSaveImage(out_path);

    ilDeleteImage(image_id);

    free(data);

    plFreeImage(&image);
    plDeleteFile(path);
}

void ExtractGameData(const char *path) {
    if(plPathExists("./" PORK_BASE_DIR)) {
        printf("please delete your ./" PORK_BASE_DIR " if you want to begin extraction again!\n");
        return;
    }

    printf("unable to find data directory\nextracting game contents from %s...\n", path);

    // Check if it's the PSX or PC version.
    char file_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(file_path, "%s/system.cnf", path);
    if(plFileExists(file_path)) {
        printf("found system.cnf, assuming psx version...\n");
        is_psx = true;
        // todo, continue here? currently unsupported!
    }

    if(!plCreateDirectory("./" PORK_BASE_DIR)) {
        printf("failed to create base directory, ./" PORK_BASE_DIR ", aborting!\n");
        return;
    }

    printf("extracting MAD/MTD packages...\n");

    sprintf(current_target, "./" PORK_MAPS_DIR);
    sprintf(file_path, "%s/Maps", path);
    plScanDirectory(file_path, "mad", ExtractMADPackage, false);
    plScanDirectory(file_path, "mtd", ExtractMADPackage, false);

    sprintf(current_target, "./" PORK_CHARS_DIR);
    sprintf(file_path, "%s/Chars", path);
    plScanDirectory(file_path, "mad", ExtractMADPackage, false);
    plScanDirectory(file_path, "mtd", ExtractMADPackage, false);

    printf("extracting PTG packages...\n");

    sprintf(current_target, "./" PORK_MAPS_DIR);
    sprintf(file_path, "%s/Maps", path);
    plScanDirectory(file_path, "ptg", ExtractPTGPackage, true);

    sprintf(current_target, "./" PORK_SKYS_DIR);
    sprintf(file_path, "%s/Skys", path);
    plScanDirectory(file_path, "ptg", ExtractPTGPackage, true);

    printf("\ncopying remaining files...\n");
    if(!is_psx) {
        sprintf(file_path, "%s/Chars/mcap.mad", path);
        if(!plCopyFile(file_path, "./" PORK_CHARS_DIR "/mcap.mad")) {
            printf("failed to copy %s!\n", file_path);
        }

        sprintf(file_path, "%s/Chars/pig.HIR", path);
        if(!plCopyFile(file_path, "./" PORK_CHARS_DIR "/pig.hir")) {
            printf("failed to copy %s!\n", file_path);
        }

        if(plCreateDirectory("./" PORK_FETEXT_DIR)) {
            sprintf(file_path, "%s/FEText", path);
            plScanDirectory(file_path, "bmp", CopyTextDirectory, false);
            plScanDirectory(file_path, "tab", CopyTextDirectory, false);
        } else {
            printf("failed to create directory, \"./" PORK_FETEXT_DIR "\"!\n");
        }

        if(plCreateDirectory("./" PORK_AUDIO_DIR)) {
            sprintf(file_path, "%s/Audio", path);
            plScanDirectory(file_path, "wav", CopyAudioDirectory, false);
        } else {
            printf("failed to create directory, \"./" PORK_AUDIO_DIR "\"!\n");
        }

        sprintf(file_path, "%s/Maps", path);
        plScanDirectory(file_path, "pog", CopyMapsDirectory, false);
        plScanDirectory(file_path, "pmg", CopyMapsDirectory, false);
        plScanDirectory(file_path, "gen", CopyMapsDirectory, false);
    }

    printf("\nextraction complete!\n\nconverting TIM to PNG...\n");

    plScanDirectory("./" PORK_BASE_DIR, "tim", ConvertTIMtoPNG, true);

    printf("conversion completed, have a nice day! (>^v^)>\n");
}
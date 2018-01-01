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
#include "engine.h"

#include <PL/platform_filesystem.h>
#include <PL/platform_image.h>

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

void ExtractPTGPackage(const char *path) {
    if(path == NULL || path[0] == '\0') { // technically, this should never, ever, ever, ever happen...
        print("encountered invalid path for PTG, aborting!\n");
        return;
    }

    char ptg_name[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(ptg_name, plGetFileName(path));
    pl_strtolower(ptg_name);

    char output_dir[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(output_dir, "%s/%s", output_path, ptg_name);
    if(!plCreatePath(output_dir)) {
        print("failed to create path %s, aborting!\n", output_dir);
        return;
    }

    FILE *file = fopen(path, "rb");
    if(file == NULL) {
        print("failed to load %s, aborting!\n", path);
        return;
    }

    FILE *out = NULL;

    uint32_t num_textures;
    if(fread(&num_textures, sizeof(uint32_t), 1, file) != 1) {
        print("invalid PTG file, failed to get number of textures!\n");
        goto ABORT_PTG;
    }

    size_t tim_size = (plGetFileSize(path) - sizeof(num_textures)) / num_textures;
    for(unsigned int i = 0; i < num_textures; ++i) {
        uint8_t tim[tim_size];
        if(fread(tim, tim_size, 1, file) != 1) {
            print("failed to read tim, aborting!\n");
            goto ABORT_PTG;
        }

        char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        sprintf(out_path, "%s/%d.tim", output_dir, i);
        out = fopen(out_path, "wb");
        if(out == NULL) {
            print("failed to open %s for writing, aborting!\n", out_path);
            goto ABORT_PTG;
        }

        //print(" %s\n", out_path);
        if(fwrite(tim, tim_size, 1, out) != 1) {
            print("failed to write %s, aborting!\n", out_path);
            goto ABORT_PTG;
        }

        pork_fclose(out);
    }

    ABORT_PTG:
    pork_fclose(out);
    pork_fclose(file);
}

void ExtractMADPackage(const char *input_path, const char *output_path) {
    if(input_path == NULL || input_path[0] == '\0') { // technically, this should never, ever, ever, ever happen...
        print("encountered invalid path for MAD, aborting!\n");
        return;
    }

    char package_name[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(package_name, plGetFileName(input_path));
    pl_strtolower(package_name);

    // output_path directory
    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(out_path, "%s/%s", output_path, package_name);
    if(!plCreatePath(out_path)) {
        print("failed to create input_path %s, aborting!\n", out_path);
        return;
    }

    FILE *file = fopen(input_path, "rb");
    if(file == NULL) {
        print("failed to load %s, aborting!\n", input_path);
        return;
    }

    char package_extension[4] = {'\0'};
    snprintf(package_extension, sizeof(package_extension), "%s", plGetFileExtension(input_path));
    pl_strtolower(package_extension);

    FILE *out_index = NULL;
    // check if it's necessary for us to produce an index file
    // since models expect each texture to have a particular
    // position within the MTD package - yay...
    if(strcmp(package_extension, "mtd") == 0) {
        char index_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        sprintf(index_path, "%s/%s.index", out_path, package_name);
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
            print("invalid index size for %s, aborting!\n", package_name);
            goto ABORT_MAD;
        }

        position = ftell(file);
        if(lowest_offset > index.offset) {
            lowest_offset = index.offset;
        }

        // this is where the fun begins...

        // how uses mixed case file-names, to make things
        // easier for linux/macos support we'll just output_path
        // everything as lowercase
        pl_strtolower(index.file);

        char file_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        sprintf(file_path, "%s/%s", out_path, index.file);

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
                print("duplicate file found for %s at %s, skipping!\n", index.file, file_path);
                continue;
            }

            // this part should never happen, but we'll check for it anyway, call me paranoid!
            print("duplicate file found for %s at %s with differing size (%d vs %zu), renaming!\n",
                  index.file, file_path, index.length, size);
            strcat(file_path, "_");
            goto CHECK_AGAIN;
        }

        data = calloc(index.length, sizeof(uint8_t));
        if(data == NULL) {
            print("failed to allocate %d bytes for export of %s, aborting!\n", index.length, index.file);
            goto ABORT_MAD;
        }

        // go and grab the data so we can export!
        fseek(file, index.offset, SEEK_SET);
        if(fread(data, sizeof(uint8_t), index.length, file) != index.length) {
            print("failed to read %s in %s, aborting!\n", index.file, package_name);
            goto ABORT_MAD;
        }

        out = fopen(file_path, "wb");
        if(out == NULL) {
            print("failed to open %s for writing, aborting!\n", file_path);
            goto ABORT_MAD;
        }

        //print(" %s\n", file_path);
        if(fwrite(data, sizeof(uint8_t), index.length, out) != index.length) {
            print("failed to write %s!\n", file_path);
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

void CopyDirectory(const char *path) {
    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(out_path, "%s/", output_path);
    if(!plCreatePath(out_path)) {
        print("%s\n", plGetError());
        return;
    }

    strcat(out_path, plGetFileName(path));
    pl_strtolower(out_path);
    //print(" %s\n", out_path);
    if(!plCopyFile(path, out_path)) {
        print("%s\n", plGetError());
    }
}

#include <IL/il.h>
#include <IL/ilu.h>

void ConvertImageToPNG(const char *path) {
    print("converting %s...\n", path);

    // figure out if the file already exists before
    // we even start trying to convert this thing
    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(out_path, path);
    strcat(out_path, ".png");
    if(plFileExists(out_path)) {
        print("tim already converted, deleting original\n");
        plDeleteFile(path);
        return;
    }

    PLImage image;
    PLresult result = (PLresult)plLoadImage(path, &image);
    if(result != PL_RESULT_SUCCESS) {
        print("failed to load \"%s\", %s, aborting!\n", path, plGetError());
        return;
    }

    const char *ext = plGetFileExtension(path);
    if(ext != NULL && ext[0] != '\0' && strcmp(ext, "tim") == 0) {
        // ensure that it's a format we're able to convert from
        if (image.format != PL_IMAGEFORMAT_RGB5A1) {
            print("unexpected pixel format in \"%s\", aborting!\n", path);
            goto ABORT;
        }

        if (!plConvertPixelFormat(&image, PL_IMAGEFORMAT_RGBA8)) {
            print("failed to convert \"%s\", %s, aborting!\n", path, plGetError());
            goto ABORT;
        }
    }

    if(!plFlipImageVertical(&image)) {
        print("failed to flip \"%s\", %s, aborting!\n", path, plGetError());
        goto ABORT;
    }

    // todo, eventually this should run through the platform lib
    ILuint image_id = ilGenImage();
    ilBindImage(image_id);
    ilTexImage(image.width, image.height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, image.data[0]);
    ilEnable(IL_FILE_OVERWRITE);
    ilSaveImage(out_path);
    ilDeleteImage(image_id);

    plFreeImage(&image);
    plDeleteFile(path);

    ABORT:
    plFreeImage(&image);
}

void ConvertImageCallback(unsigned int argc, char *argv[]) {
    ilInit();
    plScanDirectory(g_state.base_path, "tim", ConvertImageToPNG, true);
    ilShutDown();
}

void ExtractGameData(const char *path) {
    print("extracting game contents from %s...\n", path);

    char input_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    char output_path[PL_SYSTEM_MAX_PATH] = {'\0'};

    /* check if we're trying to load in the PSX version */
    sprintf(input_path, "%s/system.cnf", path);
    if(plFileExists(input_path)) {
        print_error("found system.cnf, assuming psx version...\n");
        /* todo, psx still needs a lot of thought here... I don't know if we even want to bother? */
    }

    print("copying chars data...\n");
    {
        /* animations */
        sprintf(output_path, "%s/chars/anims", g_state.base_path);
        plCreatePath(output_path);
        sprintf(input_path, "%s/Chars/mcap.mad", path);
        sprintf(output_path, "%s/chars/anims/mcap.mad", g_state.base_path);
        plCopyFile(input_path, output_path);

        /* skeleton */
        sprintf(input_path, "%s/Chars/pig.HIR", path);
        sprintf(output_path, "%s/chars/pig.hir", g_state.base_path);
        plCopyFile(input_path, output_path);

        /* british */
        sprintf(input_path, "%s/Chars/british.mad", path);
        sprintf(output_path, "%s/chars/british/", g_state.base_path);
        ExtractMADPackage(input_path, output_path);
    }

    print("copying texture data...\n");
    {
#if 0
        sprintf(output_path, "./" PORK_TEXTURES_DIR "/skys");
        sprintf(input_path, "%s/Skys", path);
        plScanDirectory(input_path, "ptg", ExtractPTGPackage, false);

        sprintf(output_path, "./" PORK_TEXTURES_DIR "/maps");
        sprintf(input_path, "%s/Maps", path);
        plScanDirectory(input_path, "ptg", ExtractPTGPackage, false);

        sprintf(output_path, "./" PORK_TEXTURES_DIR "/models");
        sprintf(input_path, "%s/Chars", path);
        plScanDirectory(input_path, "mtd", ExtractMADPackage, false);

        sprintf(output_path, "./" PORK_TEXTURES_DIR "/models/maps");
        sprintf(input_path, "%s/Maps", path);
        plScanDirectory(input_path, "mtd", ExtractMADPackage, false);

        sprintf(output_path, "./" PORK_TEXTURES_DIR "/interface");
        sprintf(input_path, "%s/Language/Tims", path);
        plScanDirectory(input_path, "mtd", ExtractMADPackage, false);
        plScanDirectory(input_path, "mad", ExtractMADPackage, false);
        plScanDirectory(input_path, "bmp", CopyDirectory, false);

        sprintf(output_path, "./" PORK_TEXTURES_DIR "/interface/title");
        sprintf(input_path, "%s/Language/Tims/Title", path);
        plScanDirectory(input_path, "bmp", CopyDirectory, false);

        sprintf(output_path, "./" PORK_TEXTURES_DIR "/interface/briefing");
        sprintf(input_path, "%s/Language/Tims/Briefing", path);
        plScanDirectory(input_path, "bmp", CopyDirectory, false);

        sprintf(output_path, "./" PORK_TEXTURES_DIR "/interface/debrief");
        sprintf(input_path, "%s/Language/Tims/Debrief", path);
        plScanDirectory(input_path, "mtd", ExtractMADPackage, false);
        plScanDirectory(input_path, "bmp", CopyDirectory, false);

        sprintf(output_path, "./" PORK_TEXTURES_DIR "/interface/map");
        sprintf(input_path, "%s/Language/Tims/PigMap", path);
        plScanDirectory(input_path, "mad", ExtractMADPackage, false);
        plScanDirectory(input_path, "bmp", CopyDirectory, false);

        if(plCreateDirectory("./" PORK_FONTS_DIR)) {
            sprintf(output_path, "./" PORK_FONTS_DIR);
            sprintf(input_path, "%s/FEText", path);
            plScanDirectory(input_path, "bmp", CopyDirectory, false);
            plScanDirectory(input_path, "tab", CopyDirectory, false);
        } else {
            print_warning("failed to create directory, \"./" PORK_FONTS_DIR "\"!\n");
        }
#endif
    }
#if 0
    print("copying audio data...\n");
    {
        sprintf(output_path, "%s" PORK_AUDIO_DIR, g_state.base_path);
        if(plCreateDirectory(output_path)) {
            sprintf(input_path, "%s/Audio", path);
            plScanDirectory(input_path, "wav", CopyDirectory, false);
        } else {
            print_warning("failed to create directory, \"%s" PORK_AUDIO_DIR "\"!\n", g_state.base_path);
        }
    }

    print("copying map data...\n");
    {
        sprintf(output_path, "%s" PORK_MAPS_DIR, g_state.base_path);
        if(plCreateDirectory(output_path)) {
            sprintf(input_path, "%s/Maps", path);
            plScanDirectory(input_path, "pog", CopyDirectory, false);
            plScanDirectory(input_path, "pmg", CopyDirectory, false);
            plScanDirectory(input_path, "gen", CopyDirectory, false);
        } else {
            print_warning("failed to create directory, \"%s" PORK_MAPS_DIR "\"!\n", g_state.base_path);
        }
    }
#endif

    print("\ncomplete\n");
}
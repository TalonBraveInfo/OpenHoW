/* OpenHoW
 * Copyright (C) 2019 Mark Sowden <markelswo@gmail.com>
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

#include <PL/platform_filesystem.h>
#include <PL/platform_console.h>

#include "../../engine/engine.h"

#undef LogInfo
#undef LogWarn
#undef Error
#define LogInfo(...)  plLogMessage(0, __VA_ARGS__)
#define LogWarn(...)  plLogMessage(1, __VA_ARGS__)
#define Error(...)    plLogMessage(2, __VA_ARGS__); exit(EXIT_FAILURE)

static FILE *ptg_fp = NULL;

static unsigned int num_textures = 0;
static char dir_textures[256][PL_SYSTEM_MAX_PATH];
static void PTG_CountDirectoryTextures(const char *path, void *userData) {
    /* ensure consistency */
    u_unused( userData );

    size_t size = plGetLocalFileSize( path );
    if(size == 0) {
        Error("Invalid file size for \"%s\", aborting!\n", path);
    }

    static size_t use_size = 0;
    if(use_size == 0) {
        use_size = size;
    }

    if(size != use_size) {
        Error("Invalid file size for \"%s\", expected %lu (%lu), aborting!\n", path, use_size, size);
    }

    /* now index it */

    LogInfo("(%d) %s\n", num_textures, path);
    strncpy(dir_textures[num_textures++], path, PL_SYSTEM_MAX_PATH);
}

static int PTG_SortTextures(const void *a, const void *b) {
    const char *filename = plGetFileName(a);
    char str_num[4];
    memset(str_num, 0, sizeof(str_num));
    strncpy(str_num, filename, strlen(filename) - 4);
    int a_num = atoi(str_num);
    filename = plGetFileName(b);
    memset(str_num, 0, sizeof(str_num));
    strncpy(str_num, filename, strlen(filename) - 4);
    int b_num = atoi(str_num);
    //LogInfo("%s\n%d\n%s\n%d\n", a, a_num, b, b_num);
    return a_num - b_num;
}

static void PTG_Extract(const char *input_path, const char *output_path) {
    if(input_path == NULL || input_path[0] == '\0') {
        Error("Encountered invalid path for PTG, aborting!\n");
    }

    char ptg_name[PL_SYSTEM_MAX_PATH] = {'\0'};
    plStripExtension(ptg_name, sizeof(ptg_name), plGetFileName(input_path));
    pl_strtolower(ptg_name);

    if(!plCreatePath(output_path)) {
        Error("Failed to create path %s, aborting!\n", output_path);
    }

    FILE *file = fopen(input_path, "rb");
    if(file == NULL) {
        Error("Failed to load %s, aborting!\n", input_path);
    }

    uint32_t num_textures;
    if(fread(&num_textures, sizeof(uint32_t), 1, file) != 1) {
        Error("invalid PTG file, failed to get number of textures!\n");
    }

    char index_path[PL_SYSTEM_MAX_PATH];
    snprintf(index_path, sizeof(index_path), "%s/.index", output_path);
    FILE* index = fopen(index_path, "w");
    if(index == NULL) {
        Error("Failed to open index for writing at \"%s\", aborting!\n", index_path);
    }

    size_t tim_size = ( plGetLocalFileSize( input_path ) - sizeof( num_textures ) ) / num_textures;
    for(unsigned int i = 0; i < num_textures; ++i) {
        fprintf(index, "%d\n", i);

        uint8_t tim[tim_size];
        if(fread(tim, tim_size, 1, file) != 1) {
            LogInfo("failed to read tim, aborting!\n");
            goto ABORT_PTG;
        }

        char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
        sprintf(out_path, "%s/%d.tim", output_path, i);
        FILE* out = fopen(out_path, "wb");
        if(out == NULL) {
            LogInfo("failed to open %s for writing, aborting!\n", out_path);
            goto ABORT_PTG;
        }

        //print(" %s\n", out_path);
        if(fwrite(tim, tim_size, 1, out) != 1) {
            LogInfo("Failed to write \"%s\", aborting!\n", out_path);
        }

        u_fclose(out);
    }

    ABORT_PTG:
    u_fclose(index);
    u_fclose(file);
}

static const char *instructions =
        "USAGE:\n"
        "   ptgtool extract \"pathtoptg\" \"outpath\"\n"
        "   ptgtool package \"pathtodir\" \"outpath\"";

int main(int argc, char **argv) {
    plInitialize(argc, argv);

    char app_dir[PL_SYSTEM_MAX_PATH];
    plGetApplicationDataDirectory(ENGINE_APP_NAME, app_dir, PL_SYSTEM_MAX_PATH);

    if(!plCreatePath(app_dir)) {
        LogWarn("Unable to create %s: %s\nSettings will not be saved.", app_dir, plGetError());
    }

    char log_path[PL_SYSTEM_MAX_PATH];
    snprintf(log_path, PL_SYSTEM_MAX_PATH, "%s/ptgtool", app_dir);
    plSetupLogOutput(log_path);

    LogInfo("Log path: %s\n", log_path);

    plSetupLogLevel(0, "info", PLColour(0, 255, 0, 255), true);
    plSetupLogLevel(1, "warning", PLColour(255, 255, 0, 255), true);
    plSetupLogLevel(2, "error", PLColour(255, 0, 0, 255), true);

    if(argc < 2) {
        printf("Invalid number of arguments!\n%s\n", instructions);
        return EXIT_SUCCESS;
    }

    char in_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};

    enum {
        MODE_NONE,
        MODE_EXTRACT,
        MODE_PACKAGE,
    } mode = MODE_NONE;

    const char *arg = NULL;
    if((arg = plGetCommandLineArgumentValue("extract")) != NULL) {
        mode = MODE_EXTRACT;
    } else if((arg = plGetCommandLineArgumentValue("package")) != NULL) {
        mode = MODE_PACKAGE;
    } else {
        Error("Invalid mode specified, aborting!\n%s\n", instructions);
    }

    strncpy(in_path, arg, sizeof(in_path));
    LogInfo("Input: %s\n", in_path);
    if(argc > 3 && (argv[3] != NULL && argv[3][0] != '\0')) {
        strncpy(out_path, argv[3], sizeof(out_path));
        LogInfo("Output: %s\n", out_path);
    } else if(mode == MODE_PACKAGE) {
        Error("Valid output path is required for packaging, aborting!\n");
    }

    if(mode == MODE_EXTRACT) {
        if(out_path[0] == '\0') {
            const char *filename = plGetFileName(in_path);
            if(filename == NULL) {
                Error("Failed to get filename from \"%s\"!\n", in_path);
            }

            snprintf(out_path, sizeof(out_path), "./%s/", filename);
        }

        PTG_Extract(in_path, out_path);
    } else {
        ptg_fp = fopen(out_path, "wb");
        if(ptg_fp == NULL) {
            Error("Failed to open \"%s\" for writing, aborting!\n", out_path);
        }

        plScanDirectory(in_path, "tim", PTG_CountDirectoryTextures, false, NULL );

        LogInfo("Found %d textures for writing into \"%s\"...\n", num_textures, out_path);

        fwrite(&num_textures, sizeof(uint32_t), 1, ptg_fp);
        qsort(dir_textures[0], num_textures, PL_SYSTEM_MAX_PATH, PTG_SortTextures);
        for(unsigned int i = 0; i < num_textures; ++i) {
            LogInfo("Writing \"%s\" (%d)\n", dir_textures[i], i);
			size_t tim_size = plGetLocalFileSize( dir_textures[ i ] );
			uint8_t buf[tim_size];
            FILE *tim_fp = fopen(dir_textures[i], "rb");
            fread(buf, sizeof(uint8_t), tim_size, tim_fp);
            fwrite(buf, sizeof(uint8_t), tim_size, ptg_fp);
            fclose(tim_fp);
        }
        fclose(ptg_fp);
    }

    LogInfo("Done!\n");

    return EXIT_SUCCESS;
}
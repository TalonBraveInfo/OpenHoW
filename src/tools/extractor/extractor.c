/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include <PL/platform_package.h>

#include "extractor.h"

/************************************************************/
/* Data Conversion */

typedef struct ModelConversionData {
  const char *mad;
  const char *mtd;
  const char *out;
} ModelConversionData;
static ModelConversionData pc_scenery_data[] = {
    {"/Maps/BAY.MAD", "/Maps/bay.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/ICE.MAD", "/Maps/ice.mtd","/campaigns/how/chars/scenery/"},
    {"/Maps/BOOM.MAD", "/Maps/boom.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/BUTE.MAD", "/Maps/bute.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/CAMP.MAD", "/Maps/camp.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/DEMO.MAD", "/Maps/demo.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/DEVI.MAD", "/Maps/devi.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/DVAL.MAD", "/Maps/dval.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/EASY.MAD", "/Maps/easy.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/ESTU.MAD", "/Maps/estu.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/FOOT.MAD", "/Maps/foot.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/GUNS.MAD", "/Maps/guns.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/HELL2.MAD", "/Maps/hell2.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/HELL3.MAD", "/Maps/hell3.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/HILLBASE.MAD", "/Maps/hillbase.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/ICEFLOW.MAD", "/Maps/iceflow.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/ICE.MAD", "/Maps/ice.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/ZULUS.MAD", "/Maps/zulus.mtd", "/campaigns/how/chars/scenery/"},
};

static void ConvertModelData(void) {
  for(unsigned int i = 0; i < plArrayElements(pc_scenery_data); ++i) {
    PLPackage *package = plLoadPackage(pc_scenery_data[i].mad, false);
    if(package == NULL) {
      continue;
    }
  }
}

/////////////////////////////////////////////////////////////
/* Extraction process for initial setup */

static void ExtractPTGPackage(const char *input_path, const char *output_path) {
  u_assert(input_path != NULL && input_path[0] != '\0', "encountered invalid path for PTG!\n");
  char ptg_name[PL_SYSTEM_MAX_PATH] = {'\0'};
  plStripExtension(ptg_name, sizeof(ptg_name), plGetFileName(input_path));
  pl_strtolower(ptg_name);

  if (!plCreatePath(output_path)) {
    LogInfo("failed to create path %s, aborting!\n", output_path);
    return;
  }

  FILE *file = fopen(input_path, "rb");
  if (file == NULL) {
    LogInfo("failed to load %s, aborting!\n", input_path);
    return;
  }

  FILE *out = NULL;
  FILE *out_index = NULL;

  char index_path[PL_SYSTEM_MAX_PATH] = {'\0'};
  snprintf(index_path, sizeof(index_path), "%s/%s.index", output_path, ptg_name);
  out_index = fopen(index_path, "w");
  if (out_index == NULL) {
    LogWarn("failed to open %s for writing!\n", index_path);
    goto ABORT_PTG;
  }

  uint32_t num_textures;
  if (fread(&num_textures, sizeof(uint32_t), 1, file) != 1) {
    LogInfo("invalid PTG file, failed to get number of textures!\n");
    goto ABORT_PTG;
  }

  size_t tim_size = (plGetFileSize(input_path) - sizeof(num_textures)) / num_textures;
  for (unsigned int i = 0; i < num_textures; ++i) {
    uint8_t tim[tim_size];
    if (fread(tim, tim_size, 1, file) != 1) {
      LogInfo("failed to read tim, aborting!\n");
      goto ABORT_PTG;
    }

    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(out_path, "%s/%d.tim", output_path, i);
    out = fopen(out_path, "wb");
    if (out == NULL) {
      LogInfo("Failed to open %s for writing, aborting!\n", out_path);
      goto ABORT_PTG;
    }

    //print(" %s\n", out_path);
    if (fwrite(tim, tim_size, 1, out) != 1) {
      LogInfo("Failed to write %s, aborting!\n", out_path);
      goto ABORT_PTG;
    }

    fprintf(out_index, "%d\n", i);

    u_fclose(out);
  }

  ABORT_PTG:
  u_fclose(out);
  u_fclose(out_index);
  u_fclose(file);
}

static void ExtractMADPackage(const char *input_path, const char *output_path) {
  u_assert(input_path != NULL && input_path[0] != '\0', "encountered invalid path for MAD!\n");
  char package_name[PL_SYSTEM_MAX_PATH] = {'\0'};
  plStripExtension(package_name, sizeof(package_name), plGetFileName(input_path));
  pl_strtolower(package_name);

  if (!plCreatePath(output_path)) {
    LogInfo("failed to create input_path %s, aborting!\n", output_path);
    return;
  }

  FILE *file = fopen(input_path, "rb");
  if (file == NULL) {
    LogInfo("failed to load %s, aborting!\n", input_path);
    return;
  }

  char package_extension[4] = {'\0'};
  snprintf(package_extension, sizeof(package_extension), "%s", plGetFileExtension(input_path));
  pl_strtolower(package_extension);

  FILE *out_index = NULL;
  // check if it's necessary for us to produce an index file
  // since models expect each texture to have a particular
  // position within the MTD package - yay...
  if (strcmp(package_extension, "mtd") == 0) {
    char index_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    snprintf(index_path, sizeof(index_path), "%s/%s.index", output_path, package_name);
    out_index = fopen(index_path, "w");
    if (out_index == NULL) {
      printf("failed to open %s for writing!\n", index_path);
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
  unsigned long position;
  do {
    MADIndex index;
    cur_index++;
    if (fread(&index, sizeof(MADIndex), 1, file) != 1) {
      LogInfo("invalid index size for %s, aborting!\n", package_name);
      goto ABORT_MAD;
    }

    position = ftell(file);
    if (lowest_offset > index.offset) {
      lowest_offset = index.offset;
    }

    // this is where the fun begins...

    // how uses mixed case file-names, to make things
    // easier for linux/macos support we'll just output_path
    // everything as lowercase
    pl_strtolower(index.file);

    char file_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(file_path, "%s/%s", output_path, index.file);

    // then check if we need to amend our index file
    if (out_index != NULL) {
      // remove the extension, since we're going to support
      // multiple formats for texture loading, and it's
      // unnecessary to read it in later when we're determining
      // what texture to load :)
      char index_file_path[PL_SYSTEM_MAX_PATH] = {'\0'};
      plStripExtension(index_file_path, sizeof(index_file_path), index.file);
      fprintf(out_index, "%s\n", index_file_path);
    }

    CHECK_AGAIN:
    // check if we're throwing out any duplicates when copying
    // everything over. technically this shouldn't occur but
    // is primarily being left here for now just for debugging
    // purposes - eventually once we're settled with both the
    // PC and PSX content, we can probably remove it
    if (plFileExists(file_path)) {
      size_t size = plGetFileSize(file_path);
      if (size == index.length) {
        //LogInfo("duplicate file found for %s at %s, skipping!\n", index.file, file_path);
        continue;
      }

      // this part should never happen, but we'll check for it anyway, call me paranoid!
      LogInfo("duplicate file found for %s at %s with differing size (%d vs %zu), renaming!\n",
              index.file, file_path, index.length, size);
      strcat(file_path, "_");
      goto CHECK_AGAIN;
    }

    if ((data = calloc(index.length, sizeof(uint8_t))) == NULL) {
      Error("failed to allocate data handle!\n");
    }

    // go and grab the data so we can export!
    fseek(file, index.offset, SEEK_SET);
    if (fread(data, sizeof(uint8_t), index.length, file) != index.length) {
      LogInfo("failed to read %s in %s, aborting!\n", index.file, package_name);
      goto ABORT_MAD;
    }

    out = fopen(file_path, "wb");
    if (out == NULL) {
      LogInfo("failed to open %s for writing, aborting!\n", file_path);
      goto ABORT_MAD;
    }

    //print(" %s\n", file_path);
    if (fwrite(data, sizeof(uint8_t), index.length, out) != index.length) {
      LogInfo("failed to write %s!\n", file_path);
      goto ABORT_MAD;
    }

    u_fclose(out);
    u_free(data);

    // return us to where we were in the file
    fseek(file, position, SEEK_SET);
  } while (position < lowest_offset);

  ABORT_MAD:
u_free(data);

  u_fclose(out_index);
  u_fclose(out);
  u_fclose(file);
}

static void ConvertImageToPNG(const char *path) {
  LogInfo("converting %s...\n", path);

  // figure out if the file already exists before
  // we even start trying to convert this thing
  char out_path[PL_SYSTEM_MAX_PATH];
  plStripExtension(out_path, sizeof(out_path), path);
  strcat(out_path, ".png");
  if (plFileExists(out_path)) {
    //LogInfo("Tim already converted, deleting original\n");
    plDeleteFile(path);
    return;
  }

  PLImage image;
  if (!plLoadImage(path, &image)) {
    LogInfo("Failed to load \"%s\", %s, aborting!\n", path, plGetError());
    return;
  }

  const char *ext = plGetFileExtension(path);
  if (ext != NULL && ext[0] != '\0' && strcmp(ext, "tim") == 0) {
    // ensure that it's a format we're able to convert from
    if (image.format != PL_IMAGEFORMAT_RGB5A1) {
      LogWarn("Unexpected pixel format in \"%s\", aborting!\n", path);
      goto ABORT;
    }

    if (!plConvertPixelFormat(&image, PL_IMAGEFORMAT_RGBA8)) {
      LogWarn("Failed to convert \"%s\", %s, aborting!\n", path, plGetError());
      goto ABORT;
    }
  }

  plReplaceImageColour(&image, PLColour(255, 0, 255, 255), PLColour(0, 0, 0, 0));

#if 0
  if(!plFlipImageVertical(&image)) {
      LogInfo("failed to flip \"%s\", %s, aborting!\n", path, plGetError());
      goto ABORT;
  }
#endif

  if (!plWriteImage(&image, out_path)) {
    LogInfo("failed to write png \"%s\", %s, aborting!\n", out_path, plGetError());
    goto ABORT;
  }

  plFreeImage(&image);
  plDeleteFile(path);

  ABORT:
  plFreeImage(&image);
}

/************************************************************/
/* Texture Merger */

typedef struct TMergeTarget {
  const char *path;
  unsigned int x, y;
} TMergeTarget;

typedef struct TMerge {
  const char *output;
  unsigned int num_textures;
  unsigned int width, height;
  TMergeTarget targets[10];
} TMerge;

static TMerge texture_targets[] = {
    {
        "/campaigns/how/frontend/dash/ang.png", 5, 152, 121, {{
                                                                  "/campaigns/how/frontend/dash/ang1.tim", 0, 0
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/ang2.tim", 64, 22
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/ang3.tim", 0, 64
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/ang4.tim", 64, 64
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/ang5.tim", 128, 31
                                                              }}
    },

    {
        "/campaigns/how/frontend/dash/clock.png", 4, 128, 96, {{
                                                                   "/campaigns/how/frontend/dash/clock01.tim", 0, 0
                                                               }, {
                                                                   "/campaigns/how/frontend/dash/clock02.tim", 64, 0
                                                               }, {
                                                                   "/campaigns/how/frontend/dash/clock03.tim", 0, 28
                                                               }, {
                                                                   "/campaigns/how/frontend/dash/clock04.tim", 64, 28
                                                               }}
    },

    {
        "/campaigns/how/frontend/dash/timer.png", 10, 256, 32, {{
                                                                    "/campaigns/how/frontend/dash/timer0.tim", 0, 0
                                                                }, {
                                                                    "/campaigns/how/frontend/dash/timer1.tim", 24, 0
                                                                }, {
                                                                    "/campaigns/how/frontend/dash/timer2.tim", 48, 0
                                                                }, {
                                                                    "/campaigns/how/frontend/dash/timer3.tim", 72, 0
                                                                }, {
                                                                    "/campaigns/how/frontend/dash/timer4.tim", 96, 0
                                                                }, {
                                                                    "/campaigns/how/frontend/dash/timer5.tim", 120, 0
                                                                }, {
                                                                    "/campaigns/how/frontend/dash/timer6.tim", 144, 0
                                                                }, {
                                                                    "/campaigns/how/frontend/dash/timer7.tim", 168, 0
                                                                }, {
                                                                    "/campaigns/how/frontend/dash/timer8.tim", 192, 0
                                                                }, {
                                                                    "/campaigns/how/frontend/dash/timer9.tim", 216, 0
                                                                }}
    },

    {
        "/campaigns/how/frontend/dash/pause.png", 8, 48, 48, {{
                                                                  "/campaigns/how/frontend/dash/pause1.tim", 0, 0
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/pause2.tim", 16, 0
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/pause3.tim", 32, 0
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/pause4.tim", 0, 16
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/pause5.tim", 32, 16
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/pause6.tim", 0, 32
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/pause7.tim", 16, 32
                                                              }, {
                                                                  "/campaigns/how/frontend/dash/pause8.tim", 32, 32
                                                              }}
    }
};

static void MergeTextureTargets(void) {
  unsigned int num_texture_targets = plArrayElements(texture_targets);
  LogInfo("merging %d texture targets...\n", num_texture_targets);
  for (unsigned int i = 0; i < num_texture_targets; ++i) {
    TMerge *merge = &texture_targets[i];

    LogInfo("generating %s\n", merge->output);

    PLImage output;
    memset(&output, 0, sizeof(PLImage));
    output.width = merge->width;
    output.height = merge->height;
    output.format = PL_IMAGEFORMAT_RGBA8;
    output.colour_format = PL_COLOURFORMAT_RGBA;
    output.levels = 1;
    output.size = plGetImageSize(output.format, output.width, output.height);

    if ((output.data = calloc(output.levels, sizeof(uint8_t *))) == NULL) {
      Error("Failed to allocate data handle!\n");
    }

    if ((output.data[0] = calloc(output.size, sizeof(uint8_t))) == NULL) {
      Error("Failed to allocate data handle!\n");
    }

    for (unsigned int j = 0; j < merge->num_textures; ++j) {
      PLImage image;
      const char *path = merge->targets[j].path;
      if (!plLoadImage(path, &image)) {
        LogWarn("Failed to find image, \"%s\", for merge!\n", merge->targets[j].path);
        continue;
      }

      plConvertPixelFormat(&image, PL_IMAGEFORMAT_RGBA8);

      LogInfo("Writing %s into %s\n", merge->targets[j].path, merge->output);

      uint8_t *pos = output.data[0] + ((merge->targets[j].y * output.width) + merge->targets[j].x) * 4;
      uint8_t *src = image.data[0];
      for (unsigned int y = 0; y < image.height; ++y) {
        memcpy(pos, src, (image.width * 4));
        src += image.width * 4;
        pos += output.width * 4;
      }

      plFreeImage(&image);
      plDeleteFile(path);
    }

    LogInfo("Writing %s\n", merge->output);
    plWriteImage(&output, merge->output);
    plFreeImage(&output);
  }
}

/************************************************************/

typedef struct IOPath {
  const char *input, *output;
} IOPath;

static IOPath pc_music_paths[] = {
#include "pc_music_paths.h"
};

static IOPath pc_copy_paths[] = {
#include "pc_copy_paths.h"
};

static IOPath pc_package_paths[] = {
#include "pc_package_paths.h"
};

static void ProcessPackagePaths(const char *in, const char *out, const IOPath *paths, unsigned int length) {
  for (unsigned int i = 0; i < length; ++i) {
    char output_path[PL_SYSTEM_MAX_PATH];
    snprintf(output_path, sizeof(output_path), "%s%s", out, paths[i].output);
    if (!plCreatePath(output_path)) {
      LogWarn("%s\n", plGetError());
      continue;
    }

    char input_path[PL_SYSTEM_MAX_PATH];
    snprintf(input_path, sizeof(input_path), "%s%s", in, paths[i].input);
    LogInfo("Copying %s to %s\n", input_path, output_path);
    const char *ext = plGetFileExtension(input_path);
    if (pl_strncasecmp(ext, "PTG", 3) == 0) {
      ExtractPTGPackage(input_path, output_path);
    } else {
      ExtractMADPackage(input_path, output_path);
    }
  }
}

static void ProcessCopyPaths(const char *in, const char *out, const IOPath *paths, unsigned int length) {
  for (unsigned int i = 0; i < length; ++i) {
    char output_path[PL_SYSTEM_MAX_PATH];
    snprintf(output_path, sizeof(output_path), "%s%s", out, paths[i].output);

    // Fudge the path if it's one of the audio tracks
    char *p = strstr(output_path, "sku1/");
    if (p != NULL) {
      strncpy(p, region_idents[version_info.region], 3);
      memmove(p + 3, p + 4, strlen(p + 4) + 1);
    }

    if (!plCreatePath(output_path)) {
      LogWarn("%s\n", plGetError());
      continue;
    }

    strncat(output_path, plGetFileName(paths[i].input), sizeof(output_path) - strlen(output_path) - 1);
    pl_strtolower(output_path);

    char input_path[PL_SYSTEM_MAX_PATH];
    snprintf(input_path, sizeof(input_path), "%s%s", in, paths[i].input);
    LogInfo("Copying %s to %s\n", input_path, output_path);
    plCopyFile(input_path, output_path);
  }
}

int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Invalid number of arguments ...\n"
           "  extractor <game_path> -<out_path>\n");
    return EXIT_SUCCESS;
  }

  plInitialize(argc, argv);

  char app_dir[PL_SYSTEM_MAX_PATH];
  plGetApplicationDataDirectory("OpenHoW", app_dir, PL_SYSTEM_MAX_PATH);

  if (!plCreatePath(app_dir)) {
    LogWarn("Unable to create %s: %s\nSettings will not be saved.", app_dir, plGetError());
  }

  char log_path[PL_SYSTEM_MAX_PATH];
  snprintf(log_path, PL_SYSTEM_MAX_PATH, "%s/extractor", app_dir);
  u_init_logs(log_path);

  /* now deal with any arguments */

  u_init_paths();

  char input_path[PL_SYSTEM_MAX_PATH] = {'\0'};
  char output_path[PL_SYSTEM_MAX_PATH];
#if 0
  if(getcwd(output_path, sizeof(output_path) - 1) == NULL) {
      strcpy(output_path, "./");
  }
#else
  strcpy(output_path, "./");
#endif

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      strncpy(output_path, argv[i] + 1, sizeof(output_path));
    } else {
      strncpy(input_path, argv[i], sizeof(input_path));
    }
  }

  if (plIsEmptyString(input_path)) {
    Error("invalid game path, aborting!\n");
    return EXIT_FAILURE;
  }

  LogInfo("\n"
          "output path: %s\n"
          "input path:  %s\n",
          output_path, input_path);

  /* ensure it's a valid version - original CD version requires
   * us to extract audio from the CD while GOG and Steam versions
   * provide OGG audio.
   *
   * PSX is a totally different story, with it's own fun adventure. */
  CheckGameVersion(input_path);
  if (version_info.region == REGION_UNKNOWN || version_info.platform == PLATFORM_UNKNOWN) {
    Error("Unknown game version, aborting!\n");
  } else if (version_info.platform == PLATFORM_PSX) {
    Error("Unsupported platform!\n");
  }

  if (version_info.platform == PLATFORM_PC || version_info.platform == PLATFORM_PC_DIGITAL) {
    ProcessPackagePaths(input_path, output_path, pc_package_paths, plArrayElements(pc_package_paths));
    ProcessCopyPaths(input_path, output_path, pc_copy_paths, plArrayElements(pc_copy_paths));

    if (version_info.platform == PLATFORM_PC_DIGITAL) {
      // They've done us the honors for the digital version
      ProcessCopyPaths(input_path, output_path, pc_music_paths, plArrayElements(pc_music_paths));
    } else {
      // todo: rip the disc...
    }
  }

  MergeTextureTargets();

  /* convert the remaining TIM textures to PNG */
  plScanDirectory(output_path, "tim", ConvertImageToPNG, true);

  LogInfo("Complete!\n");
  return EXIT_SUCCESS;
}

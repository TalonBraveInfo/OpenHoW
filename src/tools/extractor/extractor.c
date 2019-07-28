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

#include "../../shared/fac.h"

/************************************************************/
/* Data Conversion */

static void ConvertImageToPng(const char *path) {
  LogInfo("Converting %s...\n", path);

  // figure out if the file already exists before
  // we even start trying to convert this thing
  char out_path[PL_SYSTEM_MAX_PATH];
  plStripExtension(out_path, sizeof(out_path), path);
  strcat(out_path, ".png");
  if (plFileExists(out_path)) {
    LogInfo("Tim already converted, deleting \"%s\"\n", path);
    plDeleteFile(path);
    return;
  }

  PLImage image;
  if (!plLoadImage(path, &image)) {
    LogWarn("Failed to load image, \"%s\" (%s)!\n", path, plGetError());
    return;
  }

  const char *ext = plGetFileExtension(path);
  if (ext != NULL && ext[0] != '\0' && strcmp(ext, "tim") == 0) {
    // ensure that it's a format we're able to convert from
    if (image.format == PL_IMAGEFORMAT_RGB5A1) {
      if (plConvertPixelFormat(&image, PL_IMAGEFORMAT_RGBA8)) {
        plReplaceImageColour(&image, PLColour(255, 0, 255, 255), PLColour(0, 0, 0, 0));
        if (!plWriteImage(&image, out_path)) {
          LogWarn("Failed to write PNG, \"%s\" (%s)!\n", out_path, plGetError());
        }
      } else {
        LogWarn("Failed to convert \"%s\", %s, aborting!\n", path, plGetError());
      }
    } else {
      LogWarn("Unexpected pixel format in \"%s\", aborting!\n", path);
    }
  }

  if (plFileExists(out_path)) {
    plDeleteFile(path);
  }

  plFreeImage(&image);
}

typedef struct ModelConversionData {
  const char *mad;
  const char *mtd;
  const char *out;
} ModelConversionData;
static ModelConversionData pc_conversion_data[] = {
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

    {"/Chars/WEAPONS.MAD", "/Chars/WEAPONS.MTD", "/campaigns/how/chars/weapons/"},
    {"/Chars/PROPOINT.MAD", "/Chars/propoint.mtd", "/campaigns/how/chars/propoint/"},
    {"/Chars/TOP.MAD", "/Chars/TOP.MTD", "/campaigns/how/chars/top/"},
    {"/Chars/SIGHT.MAD", "/Chars/SIGHT.MTD", "/campaigns/how/chars/sight/"},

    {"/Chars/SKYDOME.MAD", NULL, "/campaigns/how/skys/"},
};

static void ConvertModelData(void) {
  for(unsigned long i = 0; i < plArrayElements(pc_conversion_data); ++i) {
    PLPackage *mad = plLoadPackage(pc_conversion_data[i].mad, true);
    if(mad == NULL) {
      LogWarn("Failed to load MAD package, \"%s\" (%s)!\n", pc_conversion_data[i].mad, plGetError());
      continue;
    }

    /* Write the files out to the destination. I'm lazy and we'll delete it once we're done anyway. */
    char model_paths[mad->table_size][PL_SYSTEM_MAX_PATH];
    unsigned int num_models = 0;
    for(unsigned int j = 0; j < mad->table_size; ++j) {
      char out[PL_SYSTEM_MAX_PATH];
      snprintf(out, sizeof(out) - 1, "%s%s", pc_conversion_data[i].out, pl_strtolower(mad->table[j].file.name));
      plWriteFile(out, mad->table[j].file.data, mad->table[j].file.size);
      const char *ext = plGetFileExtension(mad->table[j].file.name);
      if (pl_strcasecmp(ext, "fac") == 0) {
        plStripExtension(model_paths[++num_models], PL_SYSTEM_MAX_PATH - 1, out);
      }
    }

    plDestroyPackage(mad);

    /* Now we need to load each fac, fetch each index for each texture and figure out
     * the true name for that texture by comparing against the mtd. */

    PLPackage *mtd = NULL;
    if(pc_conversion_data[i].mtd != NULL) {
      mtd = plLoadPackage(pc_conversion_data[i].mtd, true);
      if(mtd == NULL) {
        LogWarn("Failed to load MTD package, \"%s\" (%s)!\n", pc_conversion_data[i].mtd, plGetError());
      }
    }

    /* and now we go through again, converting everything as we do so */
    for(unsigned int j = 0; j < num_models; ++j) {

    }
  }
}

/////////////////////////////////////////////////////////////
/* Extraction process for initial setup */

static void ExtractPtgPackage(const char *input_path, const char *output_path) {
  if (!plCreatePath(output_path)) {
    LogWarn("Failed to create path, \"%s\" (%s)!\n", output_path, plGetError());
    return;
  }

  FILE *file = fopen(input_path, "rb");
  if (file == NULL) {
    LogWarn("Failed to load PTG package, \"%s\"!\n", input_path);
    return;
  }

  uint32_t num_textures = 0;
  if (fread(&num_textures, sizeof(uint32_t), 1, file) != 1) {
    LogWarn("Invalid PTG file, failed to get number of textures!\n");
  }

  size_t tim_size = (plGetFileSize(input_path) - sizeof(num_textures)) / num_textures;
  for (unsigned int i = 0; i < num_textures; ++i) {
    uint8_t tim[tim_size];
    if (fread(tim, tim_size, 1, file) != 1) {
      LogInfo("Failed to read Tim, \"%d\"!\n", i);
      continue;
    }

    char out_path[PL_SYSTEM_MAX_PATH] = {'\0'};
    sprintf(out_path, "%s%d.tim", output_path, i);
    if(!plWriteFile(out_path, tim, tim_size)) {
      LogWarn("Failed to write file, \"%s\" (%s)!\n", out_path, plGetError());
    }
    ConvertImageToPng(out_path);
  }

  u_fclose(file);
}

static void ExtractMadPackage(const char *input_path, const char *output_path) {
  if (!plCreatePath(output_path)) {
    LogInfo("Failed to create output directory,  \"%s\"!\n", output_path);
    return;
  }

  PLPackage *package = plLoadPackage(input_path, true);
  if(package == NULL) {
    LogInfo("Failed to load %s, aborting!\n", input_path);
    return;
  }

  for(unsigned int i = 0; i < package->table_size; i++) {
    char out[PL_SYSTEM_MAX_PATH];
    snprintf(out, sizeof(out) - 1, "%s%s", output_path, pl_strtolower(package->table[i].file.name));
    if(!plWriteFile(out, package->table[i].file.data, package->table[i].file.size)) {
      LogWarn("Failed to write file, \"%s\" (%s)!\n", out, plGetError());
    }
  }

  plDestroyPackage(package);
}

/************************************************************/
/* Texture Merger */

typedef struct TextureMerge {
  const char *output;
  unsigned int num_textures;
  unsigned int width, height;
  struct {
    const char *path;
    unsigned int x, y;
  } targets[10];
} TextureMerge;
static TextureMerge texture_targets[] = {
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
  LogInfo("Merging %d texture targets...\n", num_texture_targets);
  for (unsigned int i = 0; i < num_texture_targets; ++i) {
    TextureMerge *merge = &texture_targets[i];
    LogInfo("Generating %s\n", merge->output);
    PLImage *output = plCreateImage(NULL, merge->width, merge->height, PL_COLOURFORMAT_RGBA, PL_IMAGEFORMAT_RGBA8);
    if(output == NULL) {
      LogWarn("Failed to generate texture target (%s)!\n", plGetError());
      continue;
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

      uint8_t *pos = output->data[0] + ((merge->targets[j].y * output->width) + merge->targets[j].x) * 4;
      uint8_t *src = image.data[0];
      for (unsigned int y = 0; y < image.height; ++y) {
        memcpy(pos, src, (image.width * 4));
        src += image.width * 4;
        pos += output->width * 4;
      }

      plFreeImage(&image);
      plDeleteFile(path);
    }

    LogInfo("Writing %s\n", merge->output);
    plWriteImage(output, merge->output);
    plDestroyImage(output);
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
      ExtractPtgPackage(input_path, output_path);
    } else {
      ExtractMadPackage(input_path, output_path);
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
      LogWarn("Failed to create path, \"%s\" (%s)!\n", output_path, plGetError());
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

  plRegisterStandardPackageLoaders();

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
  u_set_mod_path("how");

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
    Error("Empty game path, aborting!\n");
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

  ConvertModelData();

  LogInfo("Complete!\n");
  return EXIT_SUCCESS;
}

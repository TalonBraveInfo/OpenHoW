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

#include <PL/platform_filesystem.h>
#include <PL/platform_image.h>
#include <PL/platform_console.h>

#include "../../shared/util.h"
#include "../../shared/vtx.h"
#include "../../shared/fac.h"
#include "../../shared/no2.h"

typedef struct CopyPath {
  const char *input, *output;
} CopyPath;

static CopyPath pc_music_paths[] = {
    {"/MUSIC/Track02.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track03.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track04.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track05.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track06.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track07.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track08.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track09.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track10.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track11.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track12.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track13.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track14.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track15.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track16.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track17.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track18.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track19.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track20.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track21.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track22.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track23.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track24.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track25.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track26.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track27.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track28.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track29.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track30.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track31.ogg", "/campaigns/how/music/"},
    {"/MUSIC/Track32.ogg", "/campaigns/how/music/"},
};

static CopyPath pc_copy_paths[] = {
#include "pc_copy_paths.h"
};

static CopyPath pc_package_paths[] = {
    /* pigs */
    {"/Chars/british.mad", "/campaigns/how/chars/pigs/"},        /* actually contains all the pig models */
    {"/Chars/FACES.MTD", "/campaigns/how/chars/pigs/faces/"},  /* contains all the face textures       */
    {"/Chars/AMERICAN.MTD", "/campaigns/how/chars/pigs/american/"},
    {"/Chars/british.mtd", "/campaigns/how/chars/pigs/british/"},
    {"/Chars/FRENCH.MTD", "/campaigns/how/chars/pigs/french/"},
    {"/Chars/GERMAN.MTD", "/campaigns/how/chars/pigs/german/"},
    {"/Chars/JAPANESE.MTD", "/campaigns/how/chars/pigs/japanese/"},
    {"/Chars/RUSSIAN.MTD", "/campaigns/how/chars/pigs/russian/"},
    {"/Chars/TEAMLARD.MTD", "/campaigns/how/chars/pigs/teamlard/"},
    {"/Chars/BRITHATS.MAD", "/campaigns/how/chars/pigs/hats/"},
    {"/Chars/FHATS.MAD", "/campaigns/how/chars/pigs/hats/"},

    /* weapons */
    {"/Chars/WEAPONS.MAD", "/campaigns/how/chars/weapons/"},
    {"/Chars/WEAPONS.MTD", "/campaigns/how/chars/weapons/"},
    /* */
    {"/Chars/TOP.MAD", "/campaigns/how/chars/top/"},
    {"/Chars/TOP.MTD", "/campaigns/how/chars/top/"},
    /* zoom sights */
    {"/Chars/SIGHT.MAD", "/campaigns/how/chars/sight/"},
    {"/Chars/SIGHT.MTD", "/campaigns/how/chars/sight/"},
    /* promotion point */
    {"/Chars/PROPOINT.MAD", "/campaigns/how/chars/propoint/"},
    {"/Chars/propoint.mtd", "/campaigns/how/chars/propoint/"},
    /* sky */
    {"/Chars/SKYDOME.MAD", "/campaigns/how/skys/"},
    //{"/Chars/SKYDOME.MTD", "/campaigns/how/chars/sky/"},    /* unused ?? */
    {"/Chars/TOY.MAD", "/campaigns/how/skys/toy/"},
    {"/Chars/SPACE.MAD", "/campaigns/how/skys/space/"},
    {"/Chars/SUNSET.MAD", "/campaigns/how/skys/sunset/"},
    {"/Chars/SUNRISE.MAD", "/campaigns/how/skys/sunrise/"},
    {"/Chars/SUNNY.MAD", "/campaigns/how/skys/sunny/"},
    {"/Chars/OMINOUS.MAD", "/campaigns/how/skys/ominous/"},
    {"/Chars/DESERT.MAD", "/campaigns/how/skys/desert/"},
    {"/Chars/COLDSKY.MAD", "/campaigns/how/skys/coldsky/"},
    {"/Chars/NIGHT1.MAD", "/campaigns/how/skys/night1/"},

    /* frontend */
    {"/FEBmps/FEBMP.MAD", "/campaigns/how/frontend/bitmaps/"},
    {"/Language/Tims/FEFXTIMS.MTD", "/campaigns/how/frontend/fx/"},
    {"/Language/Tims/EXPLTIMS.MAD", "/campaigns/how/frontend/expl/"},
    {"/Language/Tims/FACETIMS.MAD", "/campaigns/how/frontend/dash/"},
    {"/Language/Tims/FLAGTIMS.MAD", "/campaigns/how/frontend/dash/"},
    {"/Language/Tims/dashtims.mad", "/campaigns/how/frontend/dash/"},
    {"/Language/Tims/MAPICONS.MTD", "/campaigns/how/frontend/map/"},
    {"/Language/Tims/MENUTIMS.MAD", "/campaigns/how/frontend/dash/menu/"},
    {"/Language/Tims/TBOXTIMS.MAD", "/campaigns/how/frontend/dash/"},

    {"/Maps/BAY.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/bay.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/ICE.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/ice.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/BOOM.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/boom.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/BUTE.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/bute.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/CAMP.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/camp.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/DEMO.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/demo.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/DEVI.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/devi.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/DVAL.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/dval.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/EASY.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/easy.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/ESTU.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/estu.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/FOOT.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/foot.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/GUNS.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/guns.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/HELL2.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/hell2.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/HELL3.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/hell3.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/HILLBASE.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/hillbase.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/ICEFLOW.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/iceflow.mtd", "/campaigns/how/chars/scenery/"},
    {"/Maps/ICE.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/ice.mtd", "/campaigns/how/chars/scenery/"},

    {"/Maps/ZULUS.MAD", "/campaigns/how/chars/scenery/"},
    {"/Maps/zulus.mtd", "/campaigns/how/chars/scenery/"},

#if 1
    {"/Maps/BAY.PTG", "/campaigns/how/maps/bay/tiles/"},
    {"/Maps/ICE.PTG", "/campaigns/how/maps/ice/tiles/"},
    {"/Maps/BOOM.PTG", "/campaigns/how/maps/boom/tiles/"},
    {"/Maps/BUTE.PTG", "/campaigns/how/maps/bute/tiles/"},
    {"/Maps/CAMP.PTG", "/campaigns/how/maps/camp/tiles/"},
    {"/Maps/DEMO.PTG", "/campaigns/how/maps/demo/tiles/"},
    {"/Maps/DEVI.PTG", "/campaigns/how/maps/devi/tiles/"},
    {"/Maps/DVAL.PTG", "/campaigns/how/maps/dval/tiles/"},
    {"/Maps/EASY.PTG", "/campaigns/how/maps/easy/tiles/"},
    {"/Maps/ESTU.PTG", "/campaigns/how/maps/estu/tiles/"},
    {"/Maps/FOOT.PTG", "/campaigns/how/maps/foot/tiles/"},
    {"/Maps/GUNS.PTG", "/campaigns/how/maps/guns/tiles/"},
    {"/Maps/KEEP.PTG", "/campaigns/how/maps/keep/tiles/"},
    {"/Maps/LAKE.PTG", "/campaigns/how/maps/lake/tiles/"},
    {"/Maps/MAZE.PTG", "/campaigns/how/maps/maze/tiles/"},
    {"/Maps/ROAD.PTG", "/campaigns/how/maps/road/tiles/"},
    {"/Maps/TWIN.PTG", "/campaigns/how/maps/twin/tiles/"},
    {"/Maps/ARCHI.PTG", "/campaigns/how/maps/archi/tiles/"},
    {"/Maps/BHILL.PTG", "/campaigns/how/maps/bhill/tiles/"},
    {"/Maps/CMASS.PTG", "/campaigns/how/maps/cmass/tiles/"},
    {"/Maps/DBOWL.PTG", "/campaigns/how/maps/dbowl/tiles/"},
    {"/Maps/DEMO2.PTG", "/campaigns/how/maps/demo2/tiles/"},
    {"/Maps/DVAL2.PTG", "/campaigns/how/maps/dval2/tiles/"},
    {"/Maps/EYRIE.PTG", "/campaigns/how/maps/eyrie/tiles/"},
    {"/Maps/FINAL.PTG", "/campaigns/how/maps/final/tiles/"},
    {"/Maps/HELL2.PTG", "/campaigns/how/maps/hell2/tiles/"},
    {"/Maps/HELL3.PTG", "/campaigns/how/maps/hell3/tiles/"},
    {"/Maps/MEDIX.PTG", "/campaigns/how/maps/medix/tiles/"},
    {"/Maps/MLAKE.PTG", "/campaigns/how/maps/mlake/tiles/"},
    {"/Maps/OASIS.PTG", "/campaigns/how/maps/oasis/tiles/"},
    {"/Maps/PLAY1.PTG", "/campaigns/how/maps/play1/tiles/"},
    {"/Maps/PLAY2.PTG", "/campaigns/how/maps/play2/tiles/"},
    {"/Maps/RIDGE.PTG", "/campaigns/how/maps/ridge/tiles/"},
    {"/Maps/SNAKE.PTG", "/campaigns/how/maps/snake/tiles/"},
    {"/Maps/ZULUS.PTG", "/campaigns/how/maps/zulus/tiles/"},
    {"/Maps/ARTGUN.PTG", "/campaigns/how/maps/artgun/tiles/"},
    {"/Maps/BRIDGE.PTG", "/campaigns/how/maps/bridge/tiles/"},
    {"/Maps/DESVAL.PTG", "/campaigns/how/maps/desval/tiles/"},
    {"/Maps/FJORDS.PTG", "/campaigns/how/maps/fjords/tiles/"},
    {"/Maps/GENMUD.PTG", "/campaigns/how/maps/genmud/tiles/"},
    {"/Maps/ISLAND.PTG", "/campaigns/how/maps/island/tiles/"},
    {"/Maps/LUNAR1.PTG", "/campaigns/how/maps/lunar1/tiles/"},
    {"/Maps/MASHED.PTG", "/campaigns/how/maps/mashed/tiles/"},
    {"/Maps/ONEWAY.PTG", "/campaigns/how/maps/oneway/tiles/"},
    {"/Maps/RUMBLE.PTG", "/campaigns/how/maps/rumble/tiles/"},
    {"/Maps/SEPIA1.PTG", "/campaigns/how/maps/sepia1/tiles/"},
    {"/Maps/SNIPER.PTG", "/campaigns/how/maps/sniper/tiles/"},
    {"/Maps/TRENCH.PTG", "/campaigns/how/maps/trench/tiles/"},
    {"/Maps/CREEPY2.PTG", "/campaigns/how/maps/creepy/tiles/"},
    {"/Maps/EMPLACE.PTG", "/campaigns/how/maps/emplace/tiles/"},
    {"/Maps/GENLAVA.PTG", "/campaigns/how/maps/genlava/tiles/"},
    {"/Maps/GENSNOW.PTG", "/campaigns/how/maps/gensnow/tiles/"},
    {"/Maps/ICEFLOW.PTG", "/campaigns/how/maps/iceflow/tiles/"},
    {"/Maps/LECPROD.PTG", "/campaigns/how/maps/lecprod/tiles/"},
    {"/Maps/SUPLINE.PTG", "/campaigns/how/maps/supline/tiles/"},
    {"/Maps/genbrack.ptg", "/campaigns/how/maps/genbrack/tiles/"},
    {"/Maps/GENCHALK.PTG", "/campaigns/how/maps/genchalk/tiles/"},
    {"/Maps/GENDESRT.PTG", "/campaigns/how/maps/gendesrt/tiles/"},
    {"/Maps/HILLBASE.PTG", "/campaigns/how/maps/hillbase/tiles/"},
    {"/Maps/LIBERATE.PTG", "/campaigns/how/maps/liberate/tiles/"},

    {"/Skys/COLD/COLD1.PTG", "/campaigns/how/skys/cold1/"},
    //{"/Skys/DESERT/DESERT1.PTG", "/campaigns/how/skys/desert1/"},
#endif
};

/////////////////////////////////////////////////////////////

typedef enum ERegion {
  REGION_UNKNOWN = -1,

  REGION_ENG,
  REGION_FRE,
  REGION_GER,
  REGION_ITA,
  REGION_RUS,
  REGION_SPA,

  MAX_REGIONS
} ERegion;
const char *region_idents[MAX_REGIONS] = {
    "eng", "fre", "ger", "ita", "rus", "spa"
};

typedef enum EPlatform {
  PLATFORM_UNKNOWN = -1,

  PLATFORM_PSX,               /* PSX version */
  PLATFORM_PC,                /* PC version */
  PLATFORM_PC_DIGITAL,        /* PC/Digital version */
} EPlatform;

typedef struct VersionInfo {
  ERegion region;
  EPlatform platform;
} VersionInfo;

VersionInfo version_info = {
    .platform = PLATFORM_UNKNOWN,
    .region = REGION_UNKNOWN
};

static void CheckGameVersion(const char *path) {
  LogInfo("checking game version...\n");

  char fcheck[PL_SYSTEM_MAX_PATH];
  snprintf(fcheck, sizeof(fcheck), "%s/system.cnf", path);
  if (plFileExists(fcheck)) {
    LogInfo("Detected system.cnf, assuming PSX version\n");
    version_info.platform = PLATFORM_PSX;
    // todo: determine region for PSX ver (probably easier than on PC)
    return;
  }

  snprintf(fcheck, sizeof(fcheck), "%s/Data/foxscale.d3d", path);
  if (!plFileExists(fcheck)) {
    LogWarn("Failed to find foxscale.d3d, unable to determine platform!\n");
    return;
  }

  LogInfo("Detected Data/foxscale.d3d, assuming PC version\n");
  version_info.platform = PLATFORM_PC;

  /* todo: need better method to determine this */
  snprintf(fcheck, sizeof(fcheck), "%s/Language/Text/fetext.bin", path);
  unsigned int fetext_size = plGetFileSize(fcheck);
  snprintf(fcheck, sizeof(fcheck), "%s/Language/Text/gtext.bin", path);
  unsigned int gtext_size = plGetFileSize(fcheck);
  if (fetext_size == 8997 && gtext_size == 4518) {
    version_info.region = REGION_GER;
  } else if (fetext_size == 8102 && gtext_size == 4112) {
    version_info.region = REGION_ENG;
  }

  snprintf(fcheck, sizeof(fcheck), "%s/MUSIC/Track02.ogg", path);
  if (plFileExists(fcheck)) {
    LogInfo("Detected MUSIC/Track02.ogg, assuming GOG version\n");
    version_info.platform = PLATFORM_PC_DIGITAL;
  }

  LogInfo("platform=%d, region=%d\n", version_info.platform, version_info.region);
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

static void ProcessPackagePaths(const char *in, const char *out, const CopyPath *paths, unsigned int length) {
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

static void ProcessCopyPaths(const char *in, const char *out, const CopyPath *paths, unsigned int length) {
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
  //plScanDirectory(output_path, "tim", ConvertImageToPNG, true);

  LogInfo("Complete!\n");
  return EXIT_SUCCESS;
}

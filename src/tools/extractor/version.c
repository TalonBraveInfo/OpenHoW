/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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

#include "extractor.h"

const char *region_idents[MAX_REGIONS] = {
    "eng", "fre", "ger", "ita", "rus", "spa"
};

VersionInfo version_info = {
    .platform = PLATFORM_UNKNOWN,
    .region = REGION_UNKNOWN
};

void CheckGameVersion(const char *path) {
  LogInfo("checking game version...\n");

  char fcheck[PL_SYSTEM_MAX_PATH];
  snprintf(fcheck, sizeof(fcheck), "%s/system.cnf", path);
  if (plFileExists(fcheck)) {
    LogInfo("Detected system.cnf, assuming PSX version\n");
    version_info.platform = PLATFORM_PSX;

#if false
    // NTSC-U:
    // - SLUS-01195  HOGS OF WAR                          [E]
    // PAL:   
    // - SLES-01041  HOGS OF WAR                          [E]
    // - SLES-02769  HOGS OF WAR                          [I]
    // - SLES-02767  HOGS OF WAR [FRONTSCHWEINE]          [G]
    // - SLES-02766  HOGS OF WAR [LES COCHONS DE GUERRE]  [F]
    // - SLES-02768  HOGS OF WAR [MARRANOS EN GUERRA]     [S]

    snprintf(fcheck, sizeof(fcheck), "%s/SLUS_011.95", path);
    if (plFileExists(fcheck)) {      
      //version_info.platform = PLATFORM_PSX_NTSC?;
      version_info.region = REGION_ENG;
    }

    snprintf(fcheck, sizeof(fcheck), "%s/SLES_010.95", path);
    if (plFileExists(fcheck)) {      
      //version_info.platform = PLATFORM_PSX_PAL?;
      version_info.region = REGION_ENG;
    }

    snprintf(fcheck, sizeof(fcheck), "%s/SLES_027.66", path);
    if (plFileExists(fcheck)) {      
      //version_info.platform = PLATFORM_PSX_PAL?;
      version_info.region = REGION_FRE;
    }

    snprintf(fcheck, sizeof(fcheck), "%s/SLES_027.67", path);
    if (plFileExists(fcheck)) {      
      //version_info.platform = PLATFORM_PSX_PAL?;
      version_info.region = REGION_GER;
    }

    snprintf(fcheck, sizeof(fcheck), "%s/SLES_027.68", path);
    if (plFileExists(fcheck)) {      
      //version_info.platform = PLATFORM_PSX_PAL?;
      version_info.region = REGION_SPA;
    }

    snprintf(fcheck, sizeof(fcheck), "%s/SLES_027.69", path);
    if (plFileExists(fcheck)) {      
      //version_info.platform = PLATFORM_PSX_PAL?;
      version_info.region = REGION_ITA;
    }

    if (version_info.region == REGION_UNKNOWN) {
      LogWarn("Failed to find any known PSX-EXE file, unable to determine region!\n");
      return;
    }
#else
    // Alternative approach reading the language.lng file.
    // Language ids:
    // 0 = ENG_GB
    // 1 = ENG_US
    // 2 = FRE (no proof)
    // 3 = GER
    // 4 = SPA (no proof)
    // 5 = ITA (no proof)

    PLFile *lgn_file = plOpenFile(path, false);
    if (lgn_file == NULL) {
      Error("Failed to load \"%s\", aborting (%s)!\n", path, plGetError());
      return;
    }

    uint32_t language_id;
    if (plReadFile(lgn_file, &language_id, sizeof(uint32_t), 1) != 1) {
      Error("Failed to get language code, \"%s\" (%s)!\n", path, plGetError());
      return;
    }

    if (language_id == 0 || language_id == 1) {
      version_info.region = REGION_ENG;
    } else if (language_id == 2) {
      version_info.region = REGION_FRE;
    } else if (language_id == 3) {
      version_info.region = REGION_GER;
    } else if (language_id == 4) {
      version_info.region = REGION_SPA;
    } else if (language_id == 5) {
      version_info.region = REGION_ITA;
    } else {
      LogWarn("language id (%d) is unknown, unable to determine region!\n", language_id);
      return;
    }
#endif
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
  unsigned int fetext_size = plGetLocalFileSize(fcheck);
  snprintf(fcheck, sizeof(fcheck), "%s/Language/Text/gtext.bin", path);
  unsigned int gtext_size = plGetLocalFileSize(fcheck);
  if (fetext_size == 8997 && gtext_size == 4518) {
    version_info.region = REGION_GER;
  } else if (fetext_size == 8102 && gtext_size == 4112) {
    version_info.region = REGION_ENG;
  } else if (fetext_size == 8391 && gtext_size == 4513) {
    version_info.region = REGION_RUS;
  }

  snprintf(fcheck, sizeof(fcheck), "%s/MUSIC/Track02.ogg", path);
  if (plFileExists(fcheck)) {
    LogInfo("Detected MUSIC/Track02.ogg, assuming GOG version\n");
    version_info.platform = PLATFORM_PC_DIGITAL;
  }

  LogInfo("platform=%d, region=%d\n", version_info.platform, version_info.region);
}

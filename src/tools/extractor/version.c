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

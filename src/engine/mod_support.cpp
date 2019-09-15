/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include "engine.h"
#include "mod_support.h"
#include "script/script_config.h"

using namespace openhow;

/* Mod Management
 *
 * Launching games and other state management
 * of the game as a whole is dealt with here.
 */

std::map<std::string, ModManifest> campaigns;
ModManifest *current_campaign = nullptr;

static ModManifest *GetModManifestByDirectory(const std::string &dir) {
  auto campaign = campaigns.find(dir);
  if (campaign != campaigns.end()) {
    return &campaign->second;
  }

  LogWarn("Failed to find campaign \"%s\"!\n");
  return nullptr;
}

/** Load in the campaign manifest and store it into a buffer.
 *
 * @param path Path to the manifest file.
 */
void Mod_RegisterCampaign(const char *path) {
  ModManifest slot;
  try {
    ScriptConfig config(path);
    slot.name = config.GetStringProperty("name");
    slot.version = config.GetStringProperty("version");
    slot.author = config.GetStringProperty("author");
  } catch (const std::exception &e) {
    LogWarn("Failed to read campaign config, \"%s\"!\n%s\n", path, e.what());
    return;
  }

  char filename[64];
  snprintf(filename, sizeof(filename), "%s", plGetFileName(path));
  slot.manifest_path = path;

  char name[32];
  plStripExtension(name, sizeof(name), plGetFileName(path));
  slot.base_directory = name;

#if _DEBUG
  LogDebug("\nRegistered Campaign\n"
           "name:    %s\n"
           "version: %s\n"
           "author:  %s\n"
           "path:    %s\n"
           "dir:     %s\n",

           slot.name.c_str(),
           slot.version.c_str(),
           slot.author.c_str(),
           slot.manifest_path.c_str(),
           slot.base_directory.c_str()
  );
#endif

  campaigns.emplace(name, slot);
}

/** Registers all of the campaigns provided under the campaigns
 *  directory.
 */
void Mod_RegisterCampaigns(void) {
  char path[PL_SYSTEM_MAX_PATH];
  snprintf(path, sizeof(path), "%s/campaigns/", u_get_base_path());
  plScanDirectory(path, "campaign", Mod_RegisterCampaign, false);
}

/** Returns a pointer to the campaign that's currently active.
 *
 * @return Pointer to the current campaign.
 */
ModManifest *Mod_GetCurrentCampaign(void) {
  return current_campaign;
}

void Mod_SetCampaign(const char *dir) {
  ModManifest *campaign = GetModManifestByDirectory(dir);
  if (campaign == nullptr) {
    LogInfo("Campaign, \"%s\", wasn't cached on launch... attempting to load!\n", dir);

    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "%s/campaigns/%s.campaign", u_get_base_path(), dir);
    if (plFileExists(path)) {
      Mod_RegisterCampaign(path);
      campaign = GetModManifestByDirectory(dir);
    }

    if (campaign == nullptr) {
      LogWarn("Campaign \"%s\" doesn't exist!\n", dir);
      return;
    }
  }

  u_set_mod_path(campaign->base_directory.c_str());

  LogInfo("Campaign has been set to \"%s\" successfully!\n", campaign->name.c_str());
}

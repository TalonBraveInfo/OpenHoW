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
#include "MapManager.h"
#include "Map.h"
#include "script/ScriptConfig.h"

MapManager::MapManager() {
    plRegisterConsoleCommand("map", MapCommand, "");
    plRegisterConsoleCommand("maps", MapsCommand, "");

    RegisterManifests();
}

MapManager::~MapManager() {
    manifests_.clear();
}

void MapManager::RegisterManifest(const std::string &path) {
    LogInfo("Registering map \"%s\"...\n", path.c_str());

    MapManifest manifest;
    manifest.path = path;
    try {
        ScriptConfig config(path);
        manifest.name = config.GetStringProperty("name");
        manifest.description = config.GetStringProperty("description");
        manifest.sky = config.GetStringProperty("sky");

        std::vector<std::string> modes = config.GetArrayStrings("modes");
        for(const auto &mode : modes) {
            manifest.flags |= GetModeFlags(mode);
        }
    } catch(const std::exception &e) {
        LogWarn("Failed to read map config, \"%s\"!\n%s\n", path.c_str(), e.what());
    }

    char temp_buf[64];
    plStripExtension(temp_buf, sizeof(temp_buf), plGetFileName(path.c_str()));

    manifests_.insert(std::make_pair(temp_buf, manifest));
}

static void RegisterManifestInterface(const char *path) {
    MapManager::GetInstance()->RegisterManifest(path);
}

void MapManager::RegisterManifests() {
    manifests_.clear();

    std::string scan_path = std::string(GetBasePath()) + "/campaigns/" + GetCampaignPath() + "/maps";
    plScanDirectory(scan_path.c_str(), "map", RegisterManifestInterface, false);
}

MapManifest *MapManager::GetManifest(const std::string &name) {
    auto manifest = manifests_.find(name);
    if(manifest != manifests_.end()) {
        return &manifest->second;
    }

    LogWarn("Failed to get manifest for \"%s\"!\n", name.c_str());
    return nullptr;
}

unsigned int MapManager::GetModeFlags(const std::string &mode) {
    if(mode == "dm") return MAP_MODE_DEATHMATCH;
    if(mode == "sp") return MAP_MODE_SINGLEPLAYER;
    if(mode == "se") return MAP_MODE_SURVIVAL_EXPERT;
    if(mode == "sn") return MAP_MODE_SURVIVAL_NOVICE;
    if(mode == "ss") return MAP_MODE_SURVIVAL_STRATEGY;
    if(mode == "ge") return MAP_MODE_GENERATED;
    if(mode == "tm") return MAP_MODE_TRAINING;

    LogWarn("Unknown mode type \"%s\", defaulting to \"dm\"!\n", mode.c_str());
    return MAP_MODE_DEATHMATCH;
}

void MapManager::MapCommand(unsigned int argc, char **argv) {
    if(argc < 2) {
        LogWarn("Invalid number of arguments, ignoring!\n");
        return;
    }

    GameModeSetup mode;
    memset(&mode, 0, sizeof(GameModeSetup));

    mode.game_mode = MAP_MODE_DEATHMATCH;
    if(argc > 2) {
        const char *cmd_mode = argv[2];
        mode.game_mode = GetInstance()->GetModeFlags(cmd_mode);
    }

    mode.num_players = 2;
    mode.teams[0] = TEAM_BRITISH;
    mode.teams[1] = TEAM_AMERICAN;

    snprintf(mode.map, sizeof(mode.map), "%s", argv[1]);

    Game_StartNewGame(&mode);
}

void MapManager::MapsCommand(unsigned int argc, char **argv) {
    if(GetInstance()->manifests_.empty()) {
        LogWarn("No maps available!\n");
        return;
    }

    for(auto manifest : GetInstance()->manifests_) {
        MapManifest *desc = &manifest.second;
        std::string out = desc->name + " : " + desc->description + " : " + std::to_string(desc->flags);
        LogInfo("%s\n", out.c_str());
    }

    LogInfo("%u maps\n", GetInstance()->manifests_.size());
}

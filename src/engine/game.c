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
#include "game.h"
#include "map.h"
#include "language.h"

#include "script/script.h"

#include "client/frontend.h"

/* Gamemode Management
 *
 * Launching games and other state management
 * of the game as a whole is dealt with here.
 *
 * todo:
 *  > LoadCampaign should index the loaded campaign, so we can access it later [DONE]
 *  > Should scan through campaigns directory for any included campaigns [DONE]
 *  > Add loaded campaign path to a paths list, so we can check against multiple paths
 *    when loading in files
 */

static CampaignManifest *campaigns = NULL;
static uint num_campaigns = 0;
static uint max_campaigns = 8;

static uint cur_campaign_idx = (uint) -1;

CampaignManifest *Game_GetCampaignBySlot(uint num) {
    if(num >= max_campaigns) {
        uint old_max_campaigns = max_campaigns;
        max_campaigns += 8;
        if((campaigns = realloc(campaigns, sizeof(CampaignManifest) * max_campaigns)) == NULL) {
            Error("failed to resize campaign list, aborting!\n");
        }

        memset(campaigns + old_max_campaigns, 0, sizeof(CampaignManifest) * (max_campaigns - old_max_campaigns));
    }

    return &campaigns[num];
}

CampaignManifest *Game_GetCampaignByName(const char *name) {
    for(unsigned int i = 0; i < num_campaigns; ++i) {
        if(strncmp(campaigns[i].name, name, sizeof(campaigns[i].name)) == 0) {
            return &campaigns[i];
        }
    }

    LogWarn("failed to find campaign \"%s\"!\n");
    return NULL;
}

CampaignManifest *Game_GetCampaignByDirectory(const char *dir) {
    for(unsigned int i = 0; i < num_campaigns; ++i) {
        if(strncmp(campaigns[i].dir, dir, sizeof(campaigns[i].dir)) == 0) {
            return &campaigns[i];
        }
    }

    LogWarn("failed to find campaign \"%s\"!\n");
    return NULL;
}

/** Load in the campaign manifest and store it into a buffer.
 *
 * @param path Path to the manifest file.
 */
void RegisterCampaign(const char *path) {
#if 0
    /* ensure the campaign actually exists before we proceed */
    char directory[PL_SYSTEM_MAX_PATH];
    snprintf(directory, sizeof(directory), "%s/campaigns/%s/", GetBasePath(), filename);
    if(!plPathExists(directory)) {
        LogWarn("invalid campaign path, \"%s\", ignoring!\n", directory);
        return;
    }
#endif

    FILE *fp = fopen(path, "r");
    if(fp == NULL) {
        LogWarn("failed to load \"%s\"!\n", path);
        return;
    }

    size_t length = plGetFileSize(path);
    char buf[length + 1];
    if(fread(buf, sizeof(char), length, fp) != length) {
        LogWarn("failed to read entirety of manifest!\n");
    }
    fclose(fp);
    buf[length] = '\0';

    /* pass it into our JSON parser */

    ScriptContext *ctx = Script_CreateContext();
    Script_ParseBuffer(ctx, buf);

    CampaignManifest *slot = Game_GetCampaignBySlot(num_campaigns++);
    if(slot == NULL) {
        LogWarn("failed to fetch campaign slot, hit memory limit!?\n");
        Script_DestroyContext(ctx);
        return;
    }

    snprintf(slot->name, sizeof(slot->name), "%s", Script_GetStringProperty(ctx, "name", ""));
    snprintf(slot->version, sizeof(slot->version), "%s", Script_GetStringProperty(ctx, "version", ""));
    snprintf(slot->author, sizeof(slot->author), "%s", Script_GetStringProperty(ctx, "author", ""));

    Script_DestroyContext(ctx);

    char filename[64];
    snprintf(filename, sizeof(filename), "%s", plGetFileName(path));

    strncpy(slot->path, path, sizeof(slot->path));
    strncpy(slot->dir, filename, sizeof(slot->dir));
    plStripExtension(slot->dir, sizeof(slot->name), plGetFileName(path));

#if 0
    LogDebug("name:    %s\n"
             "version: %s\n"
             "author:  %s\n"
             "path:    %s\n"
             "dir:     %s\n",

             slot->name,
             slot->version,
             slot->author,
             slot->path,
             slot->dir
    );
#endif
}

/** Registers all of the campaigns provided under the campaigns
 *  directory.
 */
void RegisterCampaigns(void) {
    campaigns = u_alloc(max_campaigns, sizeof(CampaignManifest), true);

    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "%s/campaigns/", GetBasePath());
    plScanDirectory(path, "campaign", RegisterCampaign, false);
}

/** Returns a pointer to the campaign that's currently active.
 *
 * @return Pointer to the current campaign.
 */
CampaignManifest *GetCurrentCampaign(void) {
    return &campaigns[cur_campaign_idx];
}

void SetCampaign(const char *dir) {
    CampaignManifest *campaign = Game_GetCampaignByDirectory(dir);
    if(campaign == NULL) {
        LogInfo("campaign, \"%s\", wasn't cached on launch... attempting to load!\n", dir);

        char path[PL_SYSTEM_MAX_PATH];
        snprintf(path, sizeof(path), "%s/campaigns/%s.campaign", GetBasePath(), dir);
        if(plFileExists(path)) {
            RegisterCampaign(path);
            campaign = Game_GetCampaignByDirectory(dir);
        }

        if(campaign == NULL) {
            LogWarn("campaign \"%s\" doesn't exist!\n", dir);
            return;
        }
    }

    plSetConsoleVariable(cv_campaign_path, campaign->dir);

    LogInfo("campaign has been set to \"%s\" successfully!\n", campaign->name);
}

/******************************************************/

bool game_started = false;

void Game_StartNewGame(const char *map, uint mode, uint8_t num_players, bool force_start) {
    LogDebug("starting new game...\n");

    if(force_start && game_started) {
        Game_End();
    }

    FE_SetState(FE_MODE_LOADING);

    if(Map_Load(map, mode) == false) {
        LogWarn("failed to load map, aborting game!\n");
        FE_RestoreLastState();
        return;
    }

    g_state.max_players = num_players;
    if(g_state.max_players > MAX_PLAYERS) {
        g_state.max_players = MAX_PLAYERS;
    }

    memset(g_state.players, 0, sizeof(Player) * g_state.max_players);
    for(unsigned int i = 0; i < g_state.max_players; ++i) {
        g_state.players[i].is_active = true;
    }

    /* we'll assume we're hosting */
    g_state.is_host = true;

    FE_SetState(FE_MODE_GAME);
}

void Game_End(void) {
    LogDebug("ending current game...\n");

    Map_Unload();

    FE_SetState(FE_MODE_MAIN_MENU); // todo: should take to results screen???
}

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
#include "map.h"
#include "language.h"
#include "mad.h"
#include "model.h"
#include "game.h"

#include "script/script.h"

#include "client/client.h"
#include "server/server.h"
#include "config.h"

EngineState g_state;

const char *GetBasePath(void) {
    u_assert(cv_base_path);
    return cv_base_path->s_value;
}

const char *GetCampaignPath(void) {
    u_assert(cv_campaign_path);
    return cv_campaign_path->s_value;
}

void Engine_Initialize(void) {
    LogInfo("initializing engine (%d.%d.%d)...\n",
            ENGINE_MAJOR_VERSION,
            ENGINE_MINOR_VERSION,
            ENGINE_PATCH_VERSION);

    memset(&g_state, 0, sizeof(EngineState));

    Console_Initialize();

    // check for any command line arguments

    const char *var;
    if((var = plGetCommandLineArgumentValue("-path")) != NULL) {
        if(!plPathExists(var)) {
            LogWarn("invalid path \"%s\", does not exist, ignoring!\n");
        }

        plSetConsoleVariable(cv_base_path, var);
    }

    Script_Initialize();

    /* this MUST be done after all vars have been
     * initialized, otherwise, right now, certain
     * vars will not be loaded/saved! */
    Config_Load(u_find("default.config"));
    Config_Load(Config_GetUserConfigPath());

    RegisterPackageInterfaces();
    RegisterModelInterfaces();

    /* load in the manifests */

    RegisterLanguages();
    RegisterCampaigns();

    if((var = plGetCommandLineArgumentValue("-mod")) != NULL ||
       (var = plGetCommandLineArgumentValue("-campaign")) != NULL) {
        SetCampaign(var);
    }

    /* now initialize all other sub-systems */

    Client_Initialize();
    Server_Initialize();

    CacheModelData();
    CacheMapData();

    LogInfo("base path: \"%s\"\n", GetBasePath());
    LogInfo("campaign path: \"%s/campaigns/%s\"\n", GetBasePath(), GetCampaignPath());
    LogInfo("working directory: \"%s\"\n", plGetWorkingDirectory());
}

bool Engine_IsRunning(void) {
    return true;
}

void Engine_Simulate(void) {
    g_state.sim_ticks = System_GetTicks();

    Client_Simulate();
    Server_Simulate();

    g_state.last_sim_tick = System_GetTicks();
}

void Engine_Shutdown(void) {
    Client_Shutdown();
    Server_Shutdown();
    Script_Shutdown();

    Config_Save(g_state.config_path);

    plShutdown();
}

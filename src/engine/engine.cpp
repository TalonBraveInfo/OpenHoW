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
#include "language.h"
#include "model.h"
#include "ModSupport.h"
#include "client.h"
#include "config.h"
#include "input.h"
#include "frontend.h"
#include "audio.h"
#include "MapManager.h"

#include "game/SPGameMode.h"
#include "script/script.h"
#include "loaders/loaders.h"
#include "graphics/display.h"

EngineState g_state;

const char *GetBasePath(void) {
    u_assert(cv_base_path);
    return cv_base_path->s_value;
}

const char *GetCampaignPath(void) {
    u_assert(cv_campaign_path);
    return cv_campaign_path->s_value;
}

const char *GetFullCampaignPath(void) {
    static char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "%s/campaigns/%s", cv_base_path->s_value, cv_campaign_path->s_value);
    return path;
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
    if((var = plGetCommandLineArgumentValue("-path")) != nullptr) {
        if(!plPathExists(var)) {
            LogWarn("invalid path \"%s\", does not exist, ignoring!\n");
        }

        plSetConsoleVariable(cv_base_path, var);
    }

    Script_Initialize();

    /* this MUST be done after all vars have been
     * initialized, otherwise, right now, certain
     * vars will not be loaded/saved! */
    Config_Load(Config_GetUserConfigPath());

    RegisterPackageLoaders();
    RegisterModelLoaders();

    // load in the manifests

    Languages_Initialize();
    Mod_RegisterCampaigns();

    if((var = plGetCommandLineArgumentValue("-mod")) == nullptr &&
       (var = plGetCommandLineArgumentValue("-campaign")) == nullptr) {
        // otherwise default to base campaign
        var = "how";
    }

    Mod_SetCampaign(var);

    // now initialize all other sub-systems

    Input_Initialize();
    Display_Initialize();
    AudioManager::GetInstance();
    GameManager::GetInstance();
    FE_Initialize();
    MapManager::GetInstance();

    CacheModelData();

    LogInfo("Base path:         \"%s\"\n", GetBasePath());
    LogInfo("Campaign path:     \"%s/campaigns/%s\"\n", GetBasePath(), GetCampaignPath());
    LogInfo("Working directory: \"%s\"\n", plGetWorkingDirectory());
}

#include "imgui_layer.h"

bool Engine_IsRunning(void) {
    System_PollEvents();

    static unsigned int next_tick = 0;
    if(next_tick == 0) {
        next_tick = System_GetTicks();
    }

    unsigned int loops = 0;
    while(System_GetTicks() > next_tick && loops < MAX_FRAMESKIP) {
        g_state.sys_ticks = System_GetTicks();
        g_state.sim_ticks++;

        Client_ProcessInput(); // todo: kill this

        GameManager::GetInstance()->Tick();
        AudioManager::GetInstance()->Tick();

        g_state.last_sys_tick = System_GetTicks();
        next_tick += SKIP_TICKS;
        loops++;
    }

    ImGuiImpl_SetupFrame();

    double delta_time = (double)(System_GetTicks() + SKIP_TICKS - next_tick) / (double)(SKIP_TICKS);
    Display_SetupDraw(delta_time);

    Display_DrawScene();
    Display_DrawInterface();
    Display_DrawDebug();

    ImGuiImpl_Draw();

    Display_Flush();

    return true;
}

void Engine_Shutdown(void) {
    Display_Shutdown();
    AudioManager::DestroyInstance();
    Script_Shutdown();

    Config_Save(Config_GetUserConfigPath());
}

#include "game/ActorManager.h"

// Temporary interface, since graphics sub-system is written in C :^)
extern "C" void DrawActors(void) {
    ActorManager::GetInstance()->DrawActors();
}

// Temporary interface, since graphics sub-system is written in C :^)
extern "C" void DrawMap(void) {
    if(!cv_graphics_draw_world->b_value) {
        return;
    }

    Map* map = GameManager::GetInstance()->GetCurrentMap();
    if(map == nullptr) {
        return;
    }

    map->Draw();
}


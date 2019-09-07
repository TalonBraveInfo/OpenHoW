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
#include "mod_support.h"
#include "client.h"
#include "config.h"
#include "input.h"
#include "frontend.h"
#include "audio/audio.h"

#include "game/mode_base.h"
#include "script/script.h"
#include "loaders/loaders.h"
#include "graphics/display.h"

EngineState g_state;

void Engine_Initialize(void) {
    LogInfo("initializing engine (v%d.%d.%d-%s:%s-%s)...\n",
            ENGINE_MAJOR_VERSION,
            ENGINE_MINOR_VERSION,
            ENGINE_PATCH_VERSION,
            GIT_BRANCH, GIT_COMMIT_HASH, GIT_COMMIT_COUNT);

    g_state.draw_ticks = 0;
    g_state.is_host = true;

    g_state.last_draw_ms = 0;
    g_state.last_sys_tick = 0;
    g_state.sim_ticks = 0;
    g_state.sys_ticks = 0;

    g_state.gfx.clear_colour = { 0, 0, 0, 255 };
    g_state.gfx.num_actors_drawn = 0;
    g_state.gfx.num_chunks_drawn = 0;
    g_state.gfx.num_triangles_total = 0;

    u_init_paths();

    Console_Initialize();

    // check for any command line arguments

    const char *var;
    if((var = plGetCommandLineArgumentValue("-path")) != nullptr) {
        if(!plPathExists(var)) {
            LogWarn("invalid path \"%s\", does not exist, ignoring!\n");
        }

        u_set_base_path(var);
    }

    Script_Initialize();

    /* this MUST be done after all vars have been
     * initialized, otherwise, right now, certain
     * vars will not be loaded/saved! */
    Config_Load(Config_GetUserConfigPath());

    plRegisterStandardPackageLoaders();

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
    ModelManager::GetInstance();

    LogInfo("Base path:         \"%s\"\n", u_get_base_path());
    LogInfo("Campaign path:     \"%s\"\n", u_get_full_path());
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


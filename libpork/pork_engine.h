/* OpenHoW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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
#pragma once

#include <PL/platform_graphics.h>

#include "pork.h"
#include "pork_math.h"
#include "pork_console.h"
#include "pork_player.h"

///////////////////////////////////////////////////

struct {
    struct PLCamera *camera;       // camera used for general gameplay
    struct PLCamera *ui_camera;    // camera used for UI elements, orthographic

    char config_path[PL_SYSTEM_MAX_PATH];   /* user config path */

    /* server / client logic */
    bool is_host;

    unsigned int sim_ticks;
    unsigned int last_sim_tick;

    unsigned int draw_ticks;
    unsigned int last_draw_ms;

    uint8_t max_players;            /* maximum players allowed in current game */
    Player players[MAX_PLAYERS];    /* tracks all current players */

    struct {
        PorkEdCtx current_context;
    } editor;
} g_state;

#define GetViewportWidth()  g_state.camera->viewport.w
#define GetViewportHeight() g_state.camera->viewport.h

#define GetUIViewportWidth()    640
#define GetUIViewportHeight()   480

PL_EXTERN_C

void InitEngine(void);
void ShutdownEngine(void);

void SetupDefaultCamera(void);
void SetupFrontendCamera(void);

void PreDrawPork(double delta);
void DrawPork(void);
void PostDrawPork(void);

void SimulatePork(void);

bool IsPorkRunning(void);

const char *GetBasePath(void);
const char *GetCampaignPath(void);

///////////////////////////////////////////////////

unsigned int System_GetTicks(void);

void System_DisplayMessageBox(unsigned int level, const char *msg, ...);
void System_DisplayWindow(bool fullscreen, int width, int height);

void System_SwapDisplay(void);

void System_SetWindowTitle(const char *title);
bool System_SetWindowSize(int *width, int *height, bool fs);

void System_Shutdown(void);

PL_EXTERN_C_END

#define LogInfo(...)    _print_w_function(PORK_LOG_ENGINE, __VA_ARGS__)
#define LogWarn(...)    _print_w_function(PORK_LOG_ENGINE_WARNING, __VA_ARGS__)
#define Error(...) {                                            \
    _print_w_function(PORK_LOG_ENGINE_ERROR, __VA_ARGS__);      \
    System_DisplayMessageBox(PORK_MBOX_ERROR, __VA_ARGS__); \
    exit(EXIT_FAILURE);                                         \
}

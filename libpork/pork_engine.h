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

#include <pork/pork.h>

#include <PL/platform_graphics.h>

#include "pork_math.h"
#include "pork_player.h"

#define LogInfo(...)    _print_w_function(PORK_LOG_ENGINE, __VA_ARGS__)
#define LogWarn(...)    _print_w_function(PORK_LOG_ENGINE_WARNING, __VA_ARGS__)
#define Error(...) {                                            \
    _print_w_function(PORK_LOG_ENGINE_ERROR, __VA_ARGS__);      \
    g_launcher.DisplayMessageBox(PORK_MBOX_ERROR, __VA_ARGS__); \
    exit(EXIT_FAILURE);                                         \
}

///////////////////////////////////////////////////
// LIMITS

#define BASE_WIDTH  1024
#define BASE_HEIGHT 768

///////////////////////////////////////////////////

// Functions provided by the host application
PorkLauncherInterface g_launcher;

extern PLConsoleVariable *cv_debug_mode;
extern PLConsoleVariable *cv_debug_fps;
extern PLConsoleVariable *cv_debug_skeleton;
extern PLConsoleVariable *cv_debug_input;

///////////////////////////////////////////////////

struct {
    struct PLCamera *camera;       // camera used for general gameplay
    struct PLCamera *ui_camera;    // camera used for UI elements, orthographic

    char base_path[PL_SYSTEM_MAX_PATH];

    /* server / client logic */
    bool is_host;
    bool is_dedicated;

    // viewport
    bool display_fullscreen;
    unsigned int display_width;
    unsigned int display_height;

    unsigned int sim_ticks;
    unsigned int last_sim_tick;

    unsigned int draw_ticks;
    unsigned int last_draw_ms;

    Player players[MAX_PLAYERS];
} g_state;

#define GetViewportWidth()  g_state.camera->viewport.w
#define GetViewportHeight() g_state.camera->viewport.h

#define GetUIViewportWidth()    640
#define GetUIViewportHeight()   480
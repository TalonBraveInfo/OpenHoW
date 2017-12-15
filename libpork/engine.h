/* OpenHOW
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

#include "player.h"

#define print(...)          _print_w_function(PORK_LOG_ENGINE, __VA_ARGS__)
#define print_warning(...)  _print_w_function(PORK_LOG_ENGINE_WARNING, __VA_ARGS__)
#define print_error(...) {                                      \
    _print_w_function(PORK_LOG_ENGINE_ERROR, __VA_ARGS__);      \
    g_launcher.DisplayMessageBox(PORK_MBOX_ERROR, __VA_ARGS__); \
    exit(EXIT_FAILURE);                                         \
}

///////////////////////////////////////////////////
// LIMITS

#define BASE_WIDTH  640
#define BASE_HEIGHT 480

#define MAX_BONES   32

///////////////////////////////////////////////////

// Functions provided by the host application
PorkLauncherInterface g_launcher;

///////////////////////////////////////////////////

struct {
    PLCamera *camera;       // camera used for general gameplay
    PLCamera *fly_camera;   // camera used for flybys and other fun stuff
    PLCamera *ui_camera;    // camera used for UI elements, orthographic

    // viewport
    bool display_fullscreen;
    unsigned int display_width;
    unsigned int display_height;

    Player players[MAX_PLAYERS];
} g_state;
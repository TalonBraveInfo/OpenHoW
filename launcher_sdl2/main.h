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
#include <pork/model.h>

#define TITLE   "OpenPork"
#define LOG     "debug"

#define VERSION_MAJOR   0
#define VERSION_MINOR   0

#define PSX_WIDTH   320
#define PSX_HEIGHT  240

typedef struct GlobalVars {
    bool is_psx, is_psx_mode;

    unsigned int width, height;

    PLCamera *main_camera;

    struct {
        unsigned int num_players;

        bool is_paused;
    } game;

    struct {
        unsigned int colour_rbo, depth_rbo;
        unsigned int psx_fbo;
    } gl;
} GlobalVars;

extern GlobalVars g_state;

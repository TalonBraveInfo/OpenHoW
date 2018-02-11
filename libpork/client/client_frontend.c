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
#include <PL/platform_graphics.h>
#include <PL/platform_graphics_camera.h>
#include <PL/platform_filesystem.h>
#include <pork_engine.h>

#include "pork_engine.h"
#include "client_frontend.h"
#include "client_font.h"

enum {
    FE_MODE_START,    /* start screen, e.g. press any key */
    FE_MODE_GAME,     /* in-game menu... probably cut this down? */
};
unsigned int frontend_state = FE_MODE_START;

PLTexture *fe_background    = NULL;
PLTexture *fe_title         = NULL;

void InitFrontend(void) {
    InitFonts();

    /* load in all the assets we'll be using for the frontend */

    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "%sfe/pigbkpc1.bmp", g_state.base_path);
    if((fe_background = plLoadTextureImage(path, PL_TEXTURE_FILTER_LINEAR)) == NULL) {
        Error("%s, aborting!\n", plGetError());
    }

    snprintf(path, sizeof(path), "%sfe/title/title.bmp", g_state.base_path);
    if((fe_title = plLoadTextureImage(path, PL_TEXTURE_FILTER_LINEAR)) == NULL) {
        Error("%s, aborting!\n", plGetError());
    }
}

void ProcessFrontendInput(void) {
    switch(frontend_state) {
        default:break;

        case FE_MODE_START: {

        } break;
    }
}

void DrawFrontend(void) {
    switch(frontend_state) {
        default:break;

        case FE_MODE_START: {
            plDrawTexturedRectangle(0, 0, GetViewportWidth(), GetViewportHeight(), fe_title);
        } break;
    }
}

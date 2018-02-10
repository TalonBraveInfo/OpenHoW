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

#include "pork_engine.h"
#include "client_frontend.h"
#include "client_font.h"

enum {
    MODE_MENU_START,    /* start screen, e.g. press any key */
    MODE_MENU_GAME,     /* in-game menu... probably cut this down? */
};
unsigned int menu_state = MODE_MENU_START;

PLTexture *fe_background    = NULL;
PLTexture *fe_title         = NULL;

void InitFrontend(void) {
    /* load in all the assets we'll be using for the frontend */
    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "%s/fe/pigbkpc1.bmp", g_state.base_path);
    if((fe_background = plLoadTextureImage(path, PL_TEXTURE_FILTER_LINEAR)) == NULL) {
        Error("failed to load \"%s\", aborting!\n", path);
    }

    snprintf(path, sizeof(path), "%s/fe/title/title.bmp", g_state.base_path);
    if((fe_title = plLoadTextureImage(path, PL_TEXTURE_FILTER_LINEAR)) == NULL) {
        Error("failed to load \"%s\", aborting!\n", path);
    }

    InitFonts();
}

void ProcessFrontendInput(void) {
    switch(menu_state) {
        default:break;

        case MODE_MENU_START: {

        } break;
    }
}

void DrawFrontend(void) {
    switch(menu_state) {
        default:break;

        case MODE_MENU_START: {
            plDrawTexturedRectangle(0, 0, GetViewportWidth(), GetViewportHeight(), fe_title);
        } break;
    }
}

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
#include "pork_input.h"

#include "client_frontend.h"
#include "client_font.h"

enum {
    FE_MODE_INIT,   /* menu shown during initialization */
    FE_MODE_START,  /* start screen, e.g. press any key */

    FE_MODE_MAIN_MENU,      /* 'One Player' and other options */
    FE_MODE_SELECT_TEAM,    /* Team selection */

    FE_MODE_GAME,     /* in-game menu... probably cut this down? */
};
unsigned int frontend_state = FE_MODE_INIT;

PLTexture *fe_background    = NULL;
PLTexture *fe_title         = NULL;

void FrontendInputCallback(int key, bool is_pressed) {
    if(frontend_state == FE_MODE_START) {

        return;
    }

    /* we've hit our key, we can take away this
     * callback now and carry on to whatever */
    SetKeyboardFocusCallback(NULL);
}

void InitFrontend(void) {
    /* load in all the assets we'll be using for the frontend */

    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "%sfe/pigbkpc1.bmp", g_state.base_path);
    if((fe_background = plLoadTextureImage(path, PL_TEXTURE_FILTER_LINEAR)) == NULL) {
        Error("%s, aborting!\n", plGetError());
    }

    snprintf(path, sizeof(path), "%sfe/title/titlemon.bmp", g_state.base_path);
    if((fe_title = plLoadTextureImage(path, PL_TEXTURE_FILTER_LINEAR)) == NULL) {
        Error("%s, aborting!\n", plGetError());
    }

    InitFonts();
}

void ProcessFrontendInput(void) {
    switch(frontend_state) {
        default:break;

        case FE_MODE_START: {
            /* this is... kind of a hack... but ensures that
             * nothing will take away our check for a key during
             * the 'start' screen, e.g. bringing the console up */
            SetKeyboardFocusCallback(FrontendInputCallback);
        } break;
    }
}

/* Hogs of War's menu was designed
 * with a fixed resolution in mind
 * and scales poorly with anything
 * else. For now we'll just keep
 * things fixed and worry about this
 * later. */
#define FRONTEND_MENU_WIDTH     640
#define FRONTEND_MENU_HEIGHT    480

void DrawFrontend(void) {
    /* render and handle the main menu */
    if(frontend_state != FE_MODE_GAME) {
        int c_x = (GetViewportWidth() / 2) - FRONTEND_MENU_WIDTH / 2;
        int c_y = (GetViewportHeight() / 2) - FRONTEND_MENU_HEIGHT / 2;
        switch(frontend_state) {
            default:break;

            case FE_MODE_INIT: {
                /* here we display the background for the init once,
                 * taking advantage of the fact that we're not going
                 * to clear the buffer initially - this gives us an
                 * opportunity to unload the texture and load in the
                 * final one */
                static bool is_background_drawn = false;
                if(!is_background_drawn) {
                    plDrawTexturedRectangle(c_x, c_y, FRONTEND_MENU_WIDTH, FRONTEND_MENU_HEIGHT, fe_title);
                    plDeleteTexture(fe_title, true);
                    char path[PL_SYSTEM_MAX_PATH];
                    snprintf(path, sizeof(path), "%sfe/title/title.bmp", g_state.base_path);
                    if((fe_title = plLoadTextureImage(path, PL_TEXTURE_FILTER_LINEAR)) == NULL) {
                        Error("%s, aborting!\n", plGetError());
                    }
                    is_background_drawn = true;
                }

                int l_x = c_x + (640 / 2) - 512 / 2;
                /* first we'll draw in a little rectangle representing
                 * the incomplete portion of the load */
                static bool is_load_drawn = false;
                if(!is_load_drawn) {
                    plSetBlendMode(PL_BLEND_DEFAULT);
                    plDrawRectangle(plCreateRectangle(
                            PLVector2(c_x, c_y + 464),
                            PLVector2(FRONTEND_MENU_WIDTH, 16),
                            PLColour(0, 0, 0, 150),
                            PLColour(0, 0, 0, 150),
                            PLColour(0, 0, 0, 150),
                            PLColour(0, 0, 0, 150)
                    ));
                    plSetBlendMode(PL_BLEND_DEFAULT);
                    is_load_drawn = true;
                }

                /* and now we're going to draw the loading bar. */
                plDrawRectangle(plCreateRectangle(
                        PLVector2(c_x, c_y + 464),
                        PLVector2(128, 16),
                        PL_COLOUR_INDIAN_RED,
                        PL_COLOUR_INDIAN_RED,
                        PL_COLOUR_RED,
                        PL_COLOUR_RED
                ));
            } break;

            case FE_MODE_START: {
                plDrawTexturedRectangle(0, 0, GetViewportWidth(), GetViewportHeight(), fe_title);
            } break;
        }

        return;
    }


}

/*********************************************************/

void SetFrontendState(unsigned int state) {
    frontend_state = state;
}

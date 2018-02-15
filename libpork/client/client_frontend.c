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

#include "pork_engine.h"
#include "pork_input.h"

#include "client_frontend.h"
#include "client_font.h"
#include "client_display.h"

unsigned int frontend_state = FE_MODE_INIT;

unsigned int GetFrontendState(void) {
    return frontend_state;
}

void SetFrontendState(unsigned int state) {
    if(state == frontend_state) {
        LogDebug("attempted to set debug state to an already existing state!\n");
        return;
    }

    LogDebug("changing frontend state to %u...\n", state);
    switch(state) {
        default: {
            LogWarn("invalid frontend state, %u, aborting\n", state);
            return;
        }

        case FE_MODE_MAIN_MENU: {} break;
        case FE_MODE_START: {} break;
        case FE_MODE_LOADING: {} break;
    }
    frontend_state = state;
}

/* * * * * * * * * * * * * * * * * * * * * * */

/* texture assets, these are loaded and free'd at runtime */
PLTexture *fe_background    = NULL;
PLTexture *fe_title         = NULL;
PLTexture *fe_load          = NULL;
PLTexture *fe_press         = NULL;
PLTexture *fe_any           = NULL;
PLTexture *fe_key           = NULL;

void FrontendInputCallback(int key, bool is_pressed) {
    if(frontend_state == FE_MODE_START && is_pressed) {
        /* todo, play 'ting' sound! */

        /* we've hit our key, we can take away this
         * callback now and carry on to whatever */
        SetKeyboardFocusCallback(NULL);
        SetFrontendState(FE_MODE_MAIN_MENU);
        return;
    }
}

void InitFrontend(void) {
    /* load in all the assets we'll be using for the frontend */
    fe_background = LoadBasicTexture("fe/pigbkpc1.bmp");
    fe_title = LoadBasicTexture("fe/title/titlemon.bmp");
    fe_press = LoadBasicTexture("fe/title/press.bmp");
    fe_any = LoadBasicTexture("fe/title/any.bmp");
    fe_key = LoadBasicTexture("fe/title/key.bmp");

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

void SimulateFrontend(void) {
    if(frontend_state == FE_MODE_INIT) {
        /* by this point we'll make an assumption that we're
         * done initializing, bump up the progress, draw that
         * frame and then switch over to our 'start' state */
        if(GetLoadingProgress() < 100) {
            SetLoadingProgress(100);
            return;
        }

        SetFrontendState(FE_MODE_START);
        return;
    }
}

/************************************************************/

char loading_description[256];
unsigned int loading_progress = 0;

void SetLoadingScreen(const char *name) {
    if(fe_load != NULL) {
        plDeleteTexture(fe_load, true);
    }

    char screen_path[PL_SYSTEM_MAX_PATH];
    snprintf(screen_path, sizeof(screen_path), "%sfe/briefing/%s.bmp", g_state.base_path, name);
    if(!plFileExists(screen_path)) {
        snprintf(screen_path, sizeof(screen_path), "%sfe/loadmult.bmp", g_state.base_path);
        if(!plFileExists(screen_path)) {
            LogWarn("failed to load background for loading screen, \"%s\"!", screen_path);
            return;
        }
    }

    fe_load = plLoadTextureImage(screen_path, PL_TEXTURE_FILTER_LINEAR);
    if(fe_load == NULL) {
        Error("%s, aborting!\n", plGetError());
    }
}

void SetLoadingDescription(const char *description) {
    snprintf(loading_description, sizeof(loading_description), "Loading %s ...", description);
}

void SetLoadingProgress(unsigned int progress) {
    if(progress > 100) {
        progress = 100;
    }
    loading_progress = progress;

    DrawPork(0);
}

unsigned int GetLoadingProgress(void) {
    return loading_progress;
}

/************************************************************/

/* Hogs of War's menu was designed
 * with a fixed resolution in mind
 * and scales poorly with anything
 * else. For now we'll just keep
 * things fixed and worry about this
 * later. */
#define FRONTEND_MENU_WIDTH     640
#define FRONTEND_MENU_HEIGHT    480

void DrawLoadingScreen(void) {
    int c_x = (GetViewportWidth() / 2) - FRONTEND_MENU_WIDTH / 2;
    int c_y = (GetViewportHeight() / 2) - FRONTEND_MENU_HEIGHT / 2;
    plDrawTexturedRectangle(c_x, c_y, FRONTEND_MENU_WIDTH, FRONTEND_MENU_HEIGHT, fe_load);

    static const int bar_w = 332;
    int bar_x = c_x + (640 / 2) - bar_w / 2;
    int bar_y = c_y + 449;
    plDrawRectangle(plCreateRectangle(
            PLVector2(bar_x, bar_y),
            PLVector2(loading_progress / 100 * bar_w, 20),
            PL_COLOUR_INDIAN_RED,
            PL_COLOUR_INDIAN_RED,
            PL_COLOUR_RED,
            PL_COLOUR_RED
    ));

    DrawBitmapString(g_fonts[FONT_CHARS3], bar_x + 10, bar_y + 10, 4, 1.f, loading_description);
}

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
                plDrawTexturedRectangle(c_x, c_y, FRONTEND_MENU_WIDTH, FRONTEND_MENU_HEIGHT, fe_title);
            } break;

            case FE_MODE_LOADING: {
                DrawLoadingScreen();
            } break;
        }

        return;
    }


}


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
#include <PL/platform_graphics_camera.h>

#include "pork_engine.h"
#include "pork_input.h"
#include "pork_console.h"
#include "pork_map.h"

#include "client_font.h"
#include "client_actor.h"

void InitDisplay(void) {
    if(g_launcher.DisplayWindow) {
        g_launcher.DisplayWindow(g_state.display_fullscreen, g_state.display_width, g_state.display_height);
    }

    plInitializeSubSystems(PL_SUBSYSTEM_GRAPHICS);
    plSetGraphicsMode(PL_GFX_MODE_OPENGL);

    //////////////////////////////////////////////////////////

    plSetClearColour(PLColour(0, 0, 0, 255));

    g_state.camera = plCreateCamera();
    if(g_state.camera == NULL) {
        Error("failed to create camera, aborting!\n%s\n", plGetError());
    }
    g_state.camera->mode        = PL_CAMERA_MODE_PERSPECTIVE;
    g_state.camera->bounds      = (PLAABB){{-20, -20},{20, 20}};
    g_state.camera->fov         = 90;
    g_state.camera->viewport.w  = g_state.display_width;
    g_state.camera->viewport.h  = g_state.display_height;
    g_state.camera->position    = PLVector3(0, 0, -200);

    g_state.ui_camera = plCreateCamera();
    if(g_state.ui_camera == NULL) {
        Error("failed to create ui camera, aborting!\n%s\n", plGetError());
    }
    g_state.ui_camera->mode        = PL_CAMERA_MODE_ORTHOGRAPHIC;
    g_state.ui_camera->fov         = 90;
    g_state.ui_camera->viewport.w  = g_state.display_width;
    g_state.ui_camera->viewport.h  = g_state.display_height;
    //g_state.ui_camera->viewport.r_w = 320;
    //g_state.ui_camera->viewport.r_h = 240;

    plSetCullMode(PL_CULL_NEGATIVE);
}

/* shared function */
void UpdatePorkViewport(bool fullscreen, unsigned int width, unsigned int height) {
    if(g_state.camera == NULL || g_state.ui_camera == NULL) {
        // display probably hasn't been initialised
        return;
    }

    g_state.ui_camera->viewport.w = g_state.camera->viewport.w = width;
    g_state.ui_camera->viewport.h = g_state.camera->viewport.h = height;

    ResetInputStates();
}

void DEBUGDrawSkeleton();

void DrawDebugOverlay(void) {
    if(cv_debug_mode->i_value <= 0) {
        return;
    }

    if (cv_debug_fps->b_value) {
        static unsigned int fps = 0;
        static unsigned int ms = 0;
        static unsigned int update_delay = 60;
        if (update_delay < g_state.draw_ticks && g_state.last_draw_ms > 0) {
            ms = g_state.last_draw_ms;
            fps = 1000 / ms;
            update_delay = g_state.draw_ticks + 60;
        }

        char ms_count[32];
        sprintf(ms_count, "FPS: %d (%d)", fps, ms);
        DrawBitmapString(g_fonts[FONT_SMALL], 20, GetViewportHeight() - 32, 0, 1.f, ms_count);
    }

    if (cv_debug_input->i_value > 0) {
        switch (cv_debug_input->i_value) {
            default: {
                DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, "KEYBOARD STATE");
                unsigned int x = 20, y = 50;
                for (unsigned int i = 0; i < PORK_MAX_KEYS; ++i) {
                    bool status = GetKeyState(i);
                    char key_state[64];
                    snprintf(key_state, sizeof(key_state), "%d (%s)", i, status ? "TRUE" : "FALSE");
                    DrawBitmapString(g_fonts[FONT_SMALL], x, y, 0, 1.f, key_state);
                    if (y + 15 > GetViewportHeight() - 50) {
                        x += 90;
                        y = 50;
                    } else {
                        y += 15;
                    }
                }
            }
                break;

            case 2: {
                DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, "CONTROLLER STATE");

                char button_state[64];
                snprintf(button_state, sizeof(button_state), CHAR_PSX_CROSS " (%s)",
                         GetButtonState(0, PORK_BUTTON_CROSS) ? "TRUE" : "FALSE");
                unsigned int y = 50;
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_TRIANGLE " (%s)",
                         GetButtonState(0, PORK_BUTTON_TRIANGLE) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_CIRCLE " (%s)",
                         GetButtonState(0, PORK_BUTTON_CIRCLE) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_SQUARE " (%s)",
                         GetButtonState(0, PORK_BUTTON_SQUARE) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_L1 " (%s)",
                         GetButtonState(0, PORK_BUTTON_L1) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_L2 " (%s)",
                         GetButtonState(0, PORK_BUTTON_L2) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_R1 " (%s)",
                         GetButtonState(0, PORK_BUTTON_R1) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), CHAR_PSX_R2 " (%s)",
                         GetButtonState(0, PORK_BUTTON_R2) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), "START (%s)",
                         GetButtonState(0, PORK_BUTTON_START) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), "SELECT (%s)",
                         GetButtonState(0, PORK_BUTTON_SELECT) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), "UP (%s)",
                         GetButtonState(0, PORK_BUTTON_UP) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), "DOWN (%s)",
                         GetButtonState(0, PORK_BUTTON_DOWN) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), "LEFT (%s)",
                         GetButtonState(0, PORK_BUTTON_LEFT) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);

                snprintf(button_state, sizeof(button_state), "RIGHT (%s)",
                         GetButtonState(0, PORK_BUTTON_RIGHT) ? "TRUE" : "FALSE");
                DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, button_state);
            } break;
        }
        return;
    }

    char cam_pos[32];
    snprintf(cam_pos, sizeof(cam_pos), "CAMERA POSITION: %d %d %d",
             (int)g_state.camera->position.x,
             (int)g_state.camera->position.y,
             (int)g_state.camera->position.z
    );
    DrawBitmapString(g_fonts[FONT_SMALL], 20, 20, 0, 1.f, cam_pos);
}

/* shared function */
void DrawPork(double delta) {
    g_state.draw_ticks = g_launcher.GetTicks();

    plClearBuffers(PL_BUFFER_DEPTH | PL_BUFFER_COLOUR);

    plSetupCamera(g_state.camera);

    DrawMap();
    DrawActors(delta);

    // todo, throw this out and move into DrawActors, with check for cv_debug_skeleton
    // in the future, do this through "ACTOR %s SHOW SKELETON" command?
    DEBUGDrawSkeleton();

    plSetupCamera(g_state.ui_camera);

#if 1 /* debug crap */
    plDrawBevelledBorder(20, 20, 256, 256);

    plSetBlendMode(PL_BLEND_ADDITIVE);
    plDrawRectangle((PLRectangle2D){
            (PLVector2){0, 0},
            (PLVector2){GetViewportWidth(), GetViewportHeight()},
            PLColourRGB(0, 0, 128),
            PLColourRGB(0, 0, 128),
            PLColour(0, 0, 40, 0),
            PLColour(0, 0, 40, 0),
    });
    plSetBlendMode(PL_BLEND_DISABLE);
#endif

    DrawDebugOverlay();
    DrawConsole();

    // todo, need a better name for this function
    plDrawPerspectivePOST(g_state.ui_camera);

    g_launcher.SwapWindow();

    g_state.last_draw_ms = g_launcher.GetTicks() - g_state.draw_ticks;
}
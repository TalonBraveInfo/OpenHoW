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
#include "pork_engine.h"
#include "pork_map.h"
#include "pork_font.h"

#include "server/server.h"
#include "client/client.h"
#include "client/client_actor.h"

#include <PL/platform_filesystem.h>
#include <PL/platform_graphics_camera.h>

PLConsoleVariable *cv_debug_mode = NULL;
PLConsoleVariable *cv_debug_fps = NULL;
PLConsoleVariable *cv_debug_skeleton = NULL;

//////////////////////////////////////////////////////////////////////////

void SimulatePork(void) {
    g_state.sim_ticks = g_launcher.GetTicks();

    SimulateClient();

    if(g_state.is_host) {
        SimulateServer();
    }

    g_state.last_sim_tick = g_launcher.GetTicks();
}

void DEBUGDrawSkeleton(void);

void DrawPork(double delta) {
    g_state.draw_ticks = g_launcher.GetTicks();

    plClearBuffers(PL_BUFFER_DEPTH | PL_BUFFER_COLOUR);

    g_state.camera->position = PLVector3(0, 0, -200);

    plSetupCamera(g_state.camera);

    //DrawMap();
    //DrawActors(delta);
    // todo, DrawInterface

    // todo, throw this out and move into DrawActors, with check for cv_debug_skeleton
    // in the future, do this through "ACTOR %s SHOW SKELETON" command?
    DEBUGDrawSkeleton();

    plSetupCamera(g_state.ui_camera);

#if 0 /* debug crap */
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

    if(cv_debug_fps->b_value) {
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
        DrawBitmapString(g_fonts[FONT_SMALL], 20, GetViewportHeight() - 32, 1.f, ms_count);
    }

    // todo, need a better name for this function
    plDrawPerspectivePOST(g_state.ui_camera);

    g_launcher.SwapWindow();

    g_state.last_draw_ms = g_launcher.GetTicks() - g_state.draw_ticks;
}

//////////////////////////////////////////////////////////////////////////

void DebugModeCallback(const PLConsoleVariable *variable) {
    plSetupLogLevel(PORK_LOG_DEBUG, "debug", PLColour(0, 255, 255, 255), variable->b_value);
}

//////////////////////////////////////////////////////////////////////////

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
    g_state.camera->position    = PLVector3(0, 0, 0);

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
}

void UpdatePorkViewport(bool fullscreen, unsigned int width, unsigned int height) {
    if(g_state.camera == NULL || g_state.ui_camera == NULL) {
        // display probably hasn't been initialised
        return;
    }

    g_state.ui_camera->viewport.w = g_state.camera->viewport.w = width;
    g_state.ui_camera->viewport.h = g_state.camera->viewport.h = height;
}

// extractor.c
void ExtractGameData(const char *path);
void ConvertImageCallback(unsigned int argc, char *argv[]);

// pork_console.c
void SetCommand(unsigned int argc, char *argv[]);

void InitConfig(void);
void InitPlayers(void);
void InitFonts(void);
void InitShaders(void);
void InitModels(void);

void InitPork(int argc, char **argv, PorkLauncherInterface interface) {
    plInitialize(argc, argv);

    plSetupLogLevel(PORK_LOG_ENGINE, "engine", PLColour(0, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_ENGINE_WARNING, "engine-warning", PLColour(255, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_ENGINE_ERROR, "engine-error", PLColour(255, 0, 0, 255), true);
    plSetupLogLevel(PORK_LOG_DEBUG, "debug", PLColour(0, 255, 255, 255), true); // todo, disable by default

    LogInfo("initializing pork %d.%d...\n", PORK_MAJOR_VERSION, PORK_MINOR_VERSION);

    g_launcher = interface;

    memset(&g_state, 0, sizeof(g_state));
    g_state.display_width = BASE_WIDTH;
    g_state.display_height = BASE_HEIGHT;

    // todo, disable these by default
    cv_debug_mode = plRegisterConsoleVariable(
            "debug_mode", "1", pl_int_var, DebugModeCallback, "Sets the global debug level.");
    cv_debug_fps = plRegisterConsoleVariable(
            "debug_fps", "1", pl_bool_var, NULL, "If enabled, displays FPS counter.");
    cv_debug_skeleton = plRegisterConsoleVariable(
            "debug_skeleton", "1", pl_bool_var, NULL, "If enabled, skeleton for pigs will be drawn.");

    plRegisterConsoleCommand("convert_tims", ConvertImageCallback, "Convert TIM textures to PNG");
    plRegisterConsoleCommand("set", SetCommand, "");

    InitConfig();

    sprintf(g_state.base_path, "./");
    for(int i = 1; i < argc; ++i) {
        if(pl_strncasecmp("-extract", argv[i], 8) == 0) {
            const char *parm = argv[i + 1];
            if(parm == NULL || parm[0] == '\0') {
                continue;
            } ++i;

            ExtractGameData(parm);
        } else if(pl_strncasecmp("-window", argv[i], 7) == 0) {
            g_state.display_fullscreen = false;
        } else if(pl_strncasecmp("-width", argv[i], 6) == 0) {
            const char *parm = argv[i + 1];
            if(parm == NULL || parm[0] == '\0') {
                continue;
            } ++i;

            unsigned int width = (unsigned int)strtoul(parm, NULL, 0);
            if(width == 0) {
                LogWarn("invalid width passed, ignoring!\n");
                continue;
            }
            g_state.display_width = width;
        } else if(pl_strncasecmp("-path", argv[i], 5) == 0) {
            const char *parm = argv[i + 1];
            if(parm == NULL || parm[0] == '\0') {
                continue;
            } ++i;

            if(!plPathExists(argv[i])) {
                LogWarn("invalid path \"%s\", does not exist, ignoring!\n");
            }

            strncpy(g_state.base_path, argv[i], sizeof(g_state.base_path));
        } else if(pl_strncasecmp("-height", argv[i], 7) == 0) {
            const char *parm = argv[i + 1];
            if(parm == NULL || parm[0] == '\0') {
                continue;
            } ++i;

            unsigned int height = (unsigned int)strtoul(parm, NULL, 0);
            if(height == 0) {
                LogWarn("invalid height passed, ignoring!\n");
                continue;
            }
            g_state.display_height = height;
        } else if(pl_strncasecmp("+", argv[i], 1) == 0) {
            plParseConsoleString(argv[i] + 1);
            ++i;
        } else {
            LogWarn("unknown/invalid command line argument, %s!\n", argv[i]);
        }
    }

    InitDisplay();
    InitShaders();
    InitFonts();
    InitPlayers();
    InitModels();
    InitMaps();
}

void ShutdownPork(void) {
    ClearPlayers();

    ShutdownServer();

    plShutdown();
}

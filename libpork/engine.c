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
#include "engine.h"
#include "model.h"
#include "actor.h"
#include "font.h"

void SimulatePork(unsigned int ticks) {
    g_state.ticks = ticks;

    SimulateActors();
}

void DrawMap(void);

void DrawPork(unsigned int ticks, double delta) {
    g_state.ticks = ticks;

    plClearBuffers(PL_BUFFER_DEPTH | PL_BUFFER_COLOUR);

    plSetupCamera(g_state.camera);

    DrawMap();
    DrawActors(delta);
    // todo, DrawInterface

    plSetupCamera(g_state.ui_camera);

    static unsigned int fps = 0;
    static unsigned int ms = 0;
    static unsigned int update_delay = 60;
    if(update_delay < g_state.ticks) {
        ms = g_state.ticks - g_state.last_tick;
        fps = 1000 / ms;
        update_delay = g_state.ticks + 60;
    }

    char ms_count[32];
    sprintf(ms_count, "FPS: %d (%d)", fps, ms);
    DrawBitmapString(g_fonts[FONT_SMALL], 20, GetViewportHeight() - 32, 1.f, ms_count);

    g_launcher.SwapWindow();

    g_state.last_tick = g_state.ticks;
}

//////////////////////////////////////////////////////////////////////////

void DebugModeCallback(const PLConsoleVariable *variable) {
    plSetupLogLevel(PORK_LOG_DEBUG, "debug", PLColour(0, 255, 255, 255), variable->b_value);
}

//////////////////////////////////////////////////////////////////////////

void InitDisplay(void) {
    g_launcher.DisplayWindow(g_state.display_fullscreen, g_state.display_width, g_state.display_height);

    plInitializeSubSystems(PL_SUBSYSTEM_GRAPHICS);
    plSetGraphicsMode(PL_GFX_MODE_OPENGL);

    //////////////////////////////////////////////////////////

    plSetClearColour(PLColour(0, 0, 0, 255));

    g_state.camera = plCreateCamera();
    if(g_state.camera == NULL) {
        print_error("failed to create camera, aborting!\n%s\n", plGetError());
    }
    g_state.camera->mode        = PL_CAMERA_MODE_PERSPECTIVE;
    g_state.camera->bounds      = (PLAABB){{-20, -20},{20, 20}};
    g_state.camera->fov         = 90;
    g_state.camera->viewport.w  = g_state.display_width;
    g_state.camera->viewport.h  = g_state.display_height;
    g_state.camera->position    = PLVector3(0, 0, 0);

    g_state.ui_camera = plCreateCamera();
    if(g_state.ui_camera == NULL) {
        print_error("failed to create ui camera, aborting!\n%s\n", plGetError());
    }
    g_state.ui_camera->mode        = PL_CAMERA_MODE_ORTHOGRAPHIC;
    g_state.ui_camera->fov         = 90;
    g_state.ui_camera->viewport.w  = g_state.display_width;
    g_state.ui_camera->viewport.h  = g_state.display_height;
}

void UpdatePorkViewport(bool fullscreen, unsigned int width, unsigned int height) {
    if(g_state.camera == NULL || g_state.ui_camera == NULL) {
        // display probably hasn't been initialised
        return;
    }

    g_state.ui_camera->viewport.w = g_state.camera->viewport.w = width;
    g_state.ui_camera->viewport.h = g_state.camera->viewport.h = height;
}

void ExtractGameData(const char *path);

void InitConfig(void);
void InitMaps(void);
void InitPlayers(void);
void InitActors(void);
void InitFonts(void);
void InitShaders(void);
void InitModels(void);

PLConsoleVariable *cv_debug_mode = NULL;

void InitPork(int argc, char **argv, PorkLauncherInterface interface) {
    plInitialize(argc, argv);

    plSetupLogLevel(PORK_LOG_ENGINE, "engine", PLColour(0, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_ENGINE_WARNING, "engine-warning", PLColour(255, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_ENGINE_ERROR, "engine-error", PLColour(255, 0, 0, 255), true);
    plSetupLogLevel(PORK_LOG_DEBUG, "debug", PLColour(0, 255, 255, 255), true); // todo, disable by default

    print("initializing pork %d.%d...\n", PORK_MAJOR_VERSION, PORK_MINOR_VERSION);

    g_launcher = interface;

    memset(&g_state, 0, sizeof(g_state));
    g_state.display_width = BASE_WIDTH;
    g_state.display_height = BASE_HEIGHT;

    // todo, parse config file

    InitConfig();

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
                print_warning("invalid width passed, ignoring!\n");
                continue;
            }
            g_state.display_width = width;
        } else if(pl_strncasecmp("-height", argv[i], 7) == 0) {
            const char *parm = argv[i + 1];
            if(parm == NULL || parm[0] == '\0') {
                continue;
            } ++i;

            unsigned int height = (unsigned int)strtoul(parm, NULL, 0);
            if(height == 0) {
                print_warning("invalid height passed, ignoring!\n");
                continue;
            }
            g_state.display_height = height;
        } else if(pl_strncasecmp("+", argv[i], 1) == 0) {
            // todo, pass to platform console command crap...
            // maybe store them and pass them over once all initialisation is
            // complete?
            ++i;
        } else {
            print_warning("unknown/invalid command line argument, %s!\n", argv[i]);
        }
    }

    cv_debug_mode = plRegisterConsoleVariable("debug_mode", "1", pl_int_var, DebugModeCallback, ""); // todo, disable by default

    InitDisplay();
    InitShaders();
    InitFonts();
    InitPlayers();
    InitActors();
    InitModels();
    InitMaps();

    CacheModelData();
}

void ShutdownPork(void) {
    ClearPlayers();
    ClearActors();

    plShutdown();
}

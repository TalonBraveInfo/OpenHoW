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

#include <IL/il.h>

void InitDisplay(void) {
    g_launcher.DisplayViewport(&g_state.display_fullscreen, &g_state.display_width, &g_state.display_height);

    plInitializeSubSystems(PL_SUBSYSTEM_GRAPHICS);

    if((g_state.camera = plCreateCamera()) == NULL) {
        print_error("failed to create camera, aborting!\n%s\n", plGetError());
    }
    g_state.camera->mode        = PL_CAMERA_MODE_PERSPECTIVE;
    g_state.camera->bounds      = (PLAABB){{-20, -20},{20, 20}};
    g_state.camera->fov         = 90;
    g_state.camera->viewport.w  = g_state.display_width;
    g_state.camera->viewport.h  = g_state.display_height;

    if((g_state.fly_camera = plCreateCamera()) == NULL) {
        print_error("failed to create fly camera, aborting!\n%s\n", plGetError());
    }
    g_state.fly_camera->mode        = PL_CAMERA_MODE_PERSPECTIVE;
    g_state.fly_camera->bounds      = (PLAABB){{-20, -20},{20, 20}};
    g_state.fly_camera->fov         = 90;
    g_state.fly_camera->viewport.w  = g_state.display_width;
    g_state.fly_camera->viewport.h  = g_state.display_height;

    if((g_state.ui_camera = plCreateCamera()) == NULL) {
        print_error("failed to create ui camera, aborting!\n%s\n", plGetError());
    }
    g_state.ui_camera->mode         = PL_CAMERA_MODE_ORTHOGRAPHIC;
    g_state.ui_camera->viewport.w   = g_state.display_width;
    g_state.ui_camera->viewport.h   = g_state.display_height;
}

void ExtractGameData(const char *path);

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

    for(unsigned int i = 1; i < argc; ++i) {
        if(pl_strncasecmp("-extract", argv[i], 8) == 0) {
            const char *parm = argv[i + 1];
            if(parm == NULL || parm[0] == '\0') {
                continue;
            } ++i;

            ExtractGameData(parm);
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

    InitDisplay();

    ilInit();

    CacheModelData();
}
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

void InitPork(int argc, char **argv) {
    plInitialize(argc, argv);
    plSetupLogOutput(PORK_LOG);

    plSetupLogLevel(PORK_LOG_ENGINE, "p-engine", PLColour(0, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_LAUNCHER, "p-launcher", PLColour(0, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_DEBUG, "p-debug", PLColour(0, 255, 255, 255), true); // todo, disable by default

    print("initializing pork %d.%d...\n", PORK_MAJOR_VERSION, PORK_MINOR_VERSION);

    memset(&g_state, 0, sizeof(g_state));

    ilInit();

    CacheModelData();
}

void SetMessageBoxCallback(void(*MessageBoxCallback)(const char *msg, ...)) {
    DisplayMessageBox = MessageBoxCallback;
}

void InitDisplay(void) {
    plInitializeSubSystems(PL_SUBSYSTEM_GRAPHICS);

    g_state.camera = plCreateCamera();
    if(g_state.camera == NULL) {
        print_error("failed to create camera, aborting!\n");
    }

    g_state.camera->mode    = PL_CAMERA_MODE_PERSPECTIVE;
    g_state.camera->bounds  = (PLAABB){{-20, -20},{20, 20}};
    g_state.camera->fov     = 90;

    g_state.fly_camera = plCreateCamera();

    g_state.ui_camera = plCreateCamera();
    g_state.ui_camera->mode = PL_CAMERA_MODE_ORTHOGRAPHIC;
}
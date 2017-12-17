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
#include "engine_gl.h"
#include "model.h"
#include "actor.h"

#include <IL/il.h>

void SimulatePork(void) {
    SimulateActors();
}

void DrawPork(double delta) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    plSetupCamera(g_state.camera);

    DrawActors(0);
    // todo, DrawInterface

    g_launcher.SwapWindow();
}

//////////////////////////////////////////////////////////////////////////

GLState gl_state;

void InitDisplay(void) {
    g_launcher.DisplayWindow(g_state.display_fullscreen, g_state.display_width, g_state.display_height);

    GLenum err = glewInit();
    if(err != GLEW_OK) {
        print_error("failed to initialize glew, %s, aborting!\n", glewGetErrorString(err));
    }

    plSetGraphicsMode(PL_GFX_MODE_CUSTOM);
    plInitializeSubSystems(PL_SUBSYSTEM_GRAPHICS);

    memset(&gl_state, 0, sizeof(GLState));
    gl_state.renderer = (const char*)glGetString(GL_RENDERER);
    gl_state.vendor = (const char*)glGetString(GL_VENDOR);

    glGetIntegerv(GL_MINOR_VERSION, &gl_state.version_minor);
    glGetIntegerv(GL_MAJOR_VERSION, &gl_state.version_major);

    print_debug("GL_MAJOR_VERSION(%d)\nGL_MINOR_VERSION(%d)\n", gl_state.version_major, gl_state.version_minor);
    print_debug("GL_RENDERER(%s)\n", gl_state.renderer);
    print_debug("GL_VENDOR(%s)\n", gl_state.vendor);

    glGetIntegerv(GL_NUM_EXTENSIONS, &gl_state.num_extensions);
    for(unsigned int i = 0; i < gl_state.num_extensions; ++i) {
        const GLubyte *extension = glGetStringi(GL_EXTENSIONS, i);
        sprintf(gl_state.extensions[i], "%s", extension);
        print_debug(" %s\n", gl_state.extensions[i]);
    }

    //////////////////////////////////////////////////////////

    glClearColor(1.f, 0, 0, 1.f);

    if((g_state.camera = plCreateCamera()) == NULL) {
        print_error("failed to create camera, aborting!\n%s\n", plGetError());
    }
    g_state.camera->mode        = PL_CAMERA_MODE_PERSPECTIVE;
    g_state.camera->bounds      = (PLAABB){{-20, -20},{20, 20}};
    g_state.camera->fov         = 90;
    g_state.camera->viewport.w  = g_state.display_width;
    g_state.camera->viewport.h  = g_state.display_height;
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
void InitPlayers(void);
void InitActors(void);
void InitFonts(void);

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

    ilInit();

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

    InitDisplay();
    InitFonts();
    InitPlayers();
    InitActors();

    CacheModelData();
}

void ShutdownPork(void) {
    ClearPlayers();
    ClearActors();

    plShutdown();
}

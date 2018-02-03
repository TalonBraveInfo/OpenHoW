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
#include <PL/platform_filesystem.h>

#include "pork_engine.h"
#include "pork_map.h"
#include "pork_input.h"

#include "client/client_display.h"
#include "client/client.h"

#include "server/server.h"

PLConsoleVariable *cv_debug_mode        = NULL;
PLConsoleVariable *cv_debug_fps         = NULL;
PLConsoleVariable *cv_debug_skeleton    = NULL;
PLConsoleVariable *cv_debug_input       = NULL;

//////////////////////////////////////////////////////////////////////////

void DebugModeCallback(const PLConsoleVariable *variable) {
    plSetupLogLevel(PORK_LOG_DEBUG, "debug", PLColour(0, 255, 255, 255), variable->b_value);
}

//////////////////////////////////////////////////////////////////////////

// extractor.c
void ExtractGameData(const char *path);
void ConvertImageCallback(unsigned int argc, char *argv[]);

// pork_console.c
void SetCommand(unsigned int argc, char *argv[]);

void InitConfig(void);
void InitPlayers(void);
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
    cv_debug_input = plRegisterConsoleVariable(
            "debug_input",
            "0",
            pl_int_var,
            NULL,
            "Changing this cycles between different modes of debugging input\n"
            "1: keyboard states\n"
            "2: controller states"
    );

    plRegisterConsoleCommand("convert", ConvertImageCallback, "Convert TIM textures to PNG");
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
        } else if(pl_strncasecmp("-dedicated", argv[i], 10) == 0) {
            g_state.is_dedicated = true;
        } else if(pl_strncasecmp("+", argv[i], 1) == 0) {
            plParseConsoleString(argv[i] + 1);
            ++i;
        } else {
            LogWarn("unknown/invalid command line argument, %s!\n", argv[i]);
        }
    }

    if(!g_state.is_dedicated) {
        InitClient();
    }

    InitPlayers();
    InitModels();
    InitMaps();
}

void SimulatePork(void) {
    g_state.sim_ticks = g_launcher.GetTicks();

    if(!g_state.is_dedicated) {
        SimulateClient();
    }

    if(g_state.is_host) {
        SimulateServer();
    }

    g_state.last_sim_tick = g_launcher.GetTicks();
}

void ShutdownPork(void) {
    ClearPlayers();

    if(!g_state.is_dedicated) {
        ShutdownClient();
    }

    ShutdownServer();

    plShutdown();
}

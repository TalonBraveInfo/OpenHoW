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
#include "pork_console.h"
#include "pork_language.h"

#include "script/script.h"

#include "client/client.h"
#include "server/server.h"

PLConsoleVariable *cv_debug_mode        = NULL;
PLConsoleVariable *cv_debug_fps         = NULL;
PLConsoleVariable *cv_debug_skeleton    = NULL;
PLConsoleVariable *cv_debug_input       = NULL;
PLConsoleVariable *cv_debug_cache       = NULL;

PLConsoleVariable *cv_base_path = NULL;
PLConsoleVariable *cv_mod_path  = NULL;

const char *GetBasePath(void) {
    pork_assert(cv_base_path);
    return cv_base_path->s_value;
}

const char *GetModPath(void) {
    pork_assert(cv_mod_path);
    return cv_mod_path->s_value;
}

//////////////////////////////////////////////////////////////////////////

void DebugModeCallback(const PLConsoleVariable *variable) {
    plSetupLogLevel(PORK_LOG_DEBUG, "debug", PLColour(0, 255, 255, 255), variable->b_value);
}

//////////////////////////////////////////////////////////////////////////

/* pork_extractor.c */
void ExtractGameData(const char *path);

/* pork_config.c */
void InitConfig(void);
void SaveConfig(void);
void ReadConfig(void);

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
    cv_debug_mode = plRegisterConsoleVariable("dmode", "1", pl_int_var, DebugModeCallback, "global debug level");
    cv_debug_fps = plRegisterConsoleVariable("dfps", "1", pl_bool_var, NULL, "display framerate");
    cv_debug_skeleton = plRegisterConsoleVariable("dskeleton", "0", pl_bool_var, NULL, "display pig skeletons");
    cv_debug_input = plRegisterConsoleVariable(
            "dinput",
            "0",
            pl_int_var,
            NULL,
            "changing this cycles between different modes of debugging input\n"
            "1: keyboard states\n"
            "2: controller states"
    );
    cv_debug_cache = plRegisterConsoleVariable("dcache", "0", pl_bool_var, NULL, "display memory and other info");

    cv_base_path = plRegisterConsoleVariable("base_path", "./", pl_string_var, NULL, "");
    cv_mod_path = plRegisterConsoleVariable("mod_path", "", pl_string_var, NULL, "");

    InitScripting();
    InitConsole();

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

            plSetConsoleVariable(cv_base_path, argv[i]);
        } else if(pl_strncasecmp("-mod", argv[i], 4) == 0) {
            const char *parm = argv[i + 1];
            if(plIsEmptyString(parm)) {
                continue;
            } ++i;

            plSetConsoleVariable(cv_mod_path, argv[i]);
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
    LogInfo("base path: %s\n", GetBasePath());
    LogInfo("mod path: %s%s\n", GetBasePath(), GetModPath());
    LogInfo("working directory: %s\n", plGetWorkingDirectory());

    /* load in the manifest */

    LogDebug("reading manifest...\n");

    char lang_path[PL_SYSTEM_MAX_PATH];
    strncpy(lang_path, pork_find("manifest.json"), sizeof(lang_path));
    FILE *fp = fopen(lang_path, "r");
    if(fp == NULL) {
        LogWarn("failed to load \"%s\"!\n", lang_path);
        return;
    }

    size_t length = plGetFileSize(lang_path);
    char buf[length + 1];
    if(fread(buf, sizeof(char), length, fp) != length) {
        LogWarn("failed to read entirety of language manifest!\n");
    }
    fclose(fp);
    buf[length] = '\0';

    ParseJSON(buf);

    strncpy(g_state.mod_name, GetJSONStringProperty("name"), sizeof(g_state.mod_name));
    strncpy(g_state.mod_version, GetJSONStringProperty("version"), sizeof(g_state.mod_version));

    LogDebug("Caching language data...\n");

    InitLanguage();

    FlushJSON();

    /* */

    InitClient();
    InitServer();

    InitPlayers();
    InitModels();
    InitMaps();

    InitConfig();
}

bool IsPorkRunning(void) {
    return true;
}

void SimulatePork(void) {
    g_state.sim_ticks = g_launcher.GetTicks();

    SimulateClient();
    SimulateServer();

    g_state.last_sim_tick = g_launcher.GetTicks();
}

void ShutdownPork(void) {
    ClearPlayers();

    ShutdownClient();

    ShutdownServer();
    ShutdownScripting();

    SaveConfig();

    plShutdown();
}

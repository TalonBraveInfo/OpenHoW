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
#include "pork_language.h"
#include "pork_formats.h"

#include "script/script.h"

#include "client/client.h"
#include "server/server.h"

const char *GetBasePath(void) {
    pork_assert(cv_base_path);
    return cv_base_path->s_value;
}

const char *GetModPath(void) {
    pork_assert(cv_mod_path);
    return cv_mod_path->s_value;
}

//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////

/* pork_extractor.c */
void ExtractGameData(const char *path);

/* pork_config.c */
void InitConfig(void);
void SaveConfig(const char *path);
void ReadConfig(const char *path);

void InitPlayers(void);
void InitModels(void);

void *pork_malloc(size_t size) {
    return pork_alloc(1, size, true);
}

void *pork_calloc(size_t num, size_t size) {
    return pork_alloc(num, size, true);
}

void InitPork(int argc, char **argv, EngineLauncherInterface interface) {
    g_launcher = interface;

    pl_malloc = pork_malloc;
    pl_calloc = pork_calloc;

    plInitialize(argc, argv);

    plSetupLogLevel(PORK_LOG_ENGINE, "engine", PLColour(0, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_ENGINE_WARNING, "engine-warning", PLColour(255, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_ENGINE_ERROR, "engine-error", PLColour(255, 0, 0, 255), true);
    plSetupLogLevel(PORK_LOG_DEBUG, "debug", PLColour(0, 255, 255, 255),
#ifdef _DEBUG
        true
#else
        false
#endif
    );

    LogInfo("initializing engine (PORK %d.%d)...\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION);

    memset(&g_state, 0, sizeof(g_state));

    RegisterFormatInterfaces();

    InitConsole();

    // check for any command line arguments

    const char *var;
    if((var = plGetCommandLineArgumentValue("-path")) != NULL) {
        if(!plPathExists(var)) {
            LogWarn("invalid path \"%s\", does not exist, ignoring!\n");
        }

        plSetConsoleVariable(cv_base_path, var);
    }
    if((var = plGetCommandLineArgumentValue("-mod")) != NULL) {
        // ensure the mod actually exists before we proceed
        char mod_path[PL_SYSTEM_MAX_PATH];
        snprintf(mod_path, sizeof(mod_path), "%s/mods/%s/", cv_base_path->s_value, var);
        if(plPathExists(mod_path)) {
            plSetConsoleVariable(cv_mod_path, var);
        } else {
            LogWarn("invalid mod path, \"%s\", ignoring!\n", mod_path);
        }
    }

    // deal with any console vars provided (todo: pl should deal with this?)
    for(int i = 1; i < argc; ++i) {
        if(pl_strncasecmp("+", argv[i], 1) == 0) {
            plParseConsoleString(argv[i] + 1);
            ++i;
            // todo: deal with other var arguments ... :(
        }
    }

    LogInfo("base path: %s\n", GetBasePath());
    LogInfo("mod path: %s%s\n", GetBasePath(), GetModPath());
    LogInfo("working directory: %s\n", plGetWorkingDirectory());

    InitScripting();

    /* load in the manifest */

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

    LogDebug("caching language data...\n");

    RegisterLanguages();

    FlushJSON();

    /* */

    InitConfig();

    /* todo: use bit flag for desired sub-systems */
    if(g_launcher.mode != PORK_MODE_LIMITED) {
        InitClient();
        InitServer();
        InitPlayers();
        InitModels();
        InitMaps();
    }
}

void InitPorkEditor(void) {
    InitClient();
    InitModels();
    InitMaps();
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

    SaveConfig(g_state.config_path);

    plShutdown();
}

/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include "script/script.h"

void Config_Save(const char *path) {
    FILE *fp = fopen(path, "w");
    if(fp == NULL) {
        LogWarn("failed to write config to \"%s\"!\n", path);
        return;
    }

    fprintf(fp, "{\n");

    /* get access to the console variables from the platform library */
    size_t num_c;
    PLConsoleVariable **vars;
    plGetConsoleVariables(&vars, &num_c);

    /* and NOW save them! */
    bool first = true;
    for(PLConsoleVariable **var = vars; var < num_c + vars; ++var) {
        if(!(*var)->archive) {
            continue;
        }

        if(!first) {
            fprintf(fp, ",\n");
        }

        fprintf(fp, "\t\t\"%s\":\"%s\"", (*var)->var, (*var)->value);
        first = false;
    }

    fprintf(fp, "\n}\n");
    fclose(fp);
}

void Config_Load(const char *path) {
    FILE *fp = fopen(path, "r");
    if(fp == NULL) {
        LogWarn("failed to open config, \"%s\"!\n", path);
        return;
    }

    LogInfo("found \"%s\", parsing...\n", path);

    size_t length = plGetFileSize(path);
    char buf[length + 1];
    if(fread(buf, sizeof(char), length, fp) != length) {
        LogWarn("failed to read entirety of config for \"%s\"!\n", path);
    }
    fclose(fp);

    buf[length] = '\0';

    ScriptContext *ctx = Script_CreateContext();
    Script_ParseBuffer(ctx, buf);

    size_t num_c;
    PLConsoleVariable **vars;
    plGetConsoleVariables(&vars, &num_c);
    for(PLConsoleVariable **var = vars; var < vars + num_c; ++var) {
        const char *result = Script_GetStringProperty(ctx, (*var)->var, NULL);
        if(result == NULL) {
            continue;
        }

        plSetConsoleVariable((*var), result);
    }

    Script_DestroyContext(ctx);
}

void Config_Initialize(void) {
    LogInfo("checking for config...\n");

    char out[PL_SYSTEM_MAX_PATH];
    plGetApplicationDataDirectory(ENGINE_APP_NAME, out, PL_SYSTEM_MAX_PATH);
    snprintf(g_state.config_path, sizeof(g_state.config_path), "%s/config", out);

    Config_Load(g_state.config_path);
    Config_Save(g_state.config_path);
}

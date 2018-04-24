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

char config_path[PL_SYSTEM_MAX_PATH];

void SaveConfig(void) {
    FILE *fp = fopen(config_path, "w");
    if(fp == NULL) {
        LogWarn("failed to write config to \"%s\"!\n", config_path);
        return;
    }

    fprintf(fp, "{\n");

    size_t num_c;
    PLConsoleVariable **vars;
    plGetConsoleVariables(&vars, &num_c);
    for(PLConsoleVariable **var = vars; var < vars + num_c; ++var) {
        fprintf(fp, "\t\t\"%s\":\"%s\"", (*var)->var, (*var)->value);
        if(var < vars + num_c - 1) {
            fprintf(fp, ",\n");
        } else {
            fprintf(fp, "\n");
        }
    }

    fprintf(fp, "}\n");

    fclose(fp);
}

void ReadConfig(void) {
    FILE *fp = fopen(config_path, "r");
    if(fp == NULL) {
        LogWarn("failed to open map description, %s\n", config_path);
        return;
    }

    size_t length = plGetFileSize(config_path);
    char buf[length + 1];
    if(fread(buf, sizeof(char), length, fp) != length) {
        LogWarn("failed to read entirety of map description for %s!\n", config_path);
    }
    fclose(fp);

    buf[length] = '\0';
}

void InitConfig(void) {
    LogInfo("checking for config...\n");

    snprintf(config_path, sizeof(config_path), "%s" PORK_CONFIG, GetBasePath());
    if(plFileExists(config_path)) {
        LogInfo("found \"%s\", parsing...\n", config_path);
        ReadConfig();
    } else {
        LogInfo("no config found at \"%s\", generating default\n", config_path);
        SaveConfig();
    }
}
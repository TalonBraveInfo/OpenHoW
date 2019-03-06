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

#include "engine.h"
#include "config.h"

#include "script/script.h"
#include "script/ScriptConfig.h"

const char *Config_GetUserConfigPath(void) {
    static std::string config_path;
    if(config_path.empty()) {
        char out[PL_SYSTEM_MAX_PATH];
        if(plGetApplicationDataDirectory(ENGINE_APP_NAME, out, PL_SYSTEM_MAX_PATH) == nullptr) {
            LogWarn("failed to get app data directory!\n%s\n", plGetError());
            config_path = "./user.config";
        } else {
            config_path = std::string(out) + "/user.config";
        }
    }
    return config_path.c_str();
}

void Config_Save(const char *path) {
    FILE *fp = fopen(path, "w");
    if(fp == nullptr) {
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
    try {
        ScriptConfig config(path);

        size_t num_c;
        PLConsoleVariable **vars;
        plGetConsoleVariables(&vars, &num_c);

        for(PLConsoleVariable **var = vars; var < vars + num_c; ++var) {
            std::string result = config.GetStringProperty((*var)->var);
            if(result.empty()) {
                continue;
            }

            plSetConsoleVariable((*var), result.c_str());
        }
    } catch(const std::exception &e) {
        LogWarn("Failed to read user config, \"%s\"!\n%s\n", path, e.what());
    }
}

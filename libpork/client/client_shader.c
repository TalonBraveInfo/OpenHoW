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
#include "pork_engine.h"

PLShaderProgram *shader_default = NULL;

PLShaderProgram *LoadShaderProgram(const char *name) {
    if(plIsEmptyString(name)) {
        Error("invalid name for shader program, aborting!\n");
    }

    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "%sshaders/%s.vert", g_state.base_path, name);
    PLShaderStage *vertex = plLoadShaderStage(path, PL_SHADER_TYPE_VERTEX);
    if(vertex == NULL) {
        Error("failed to load %s, aborting!\n%s", path, plGetError());
    }

    snprintf(path, sizeof(path), "%sshaders/%s.frag", g_state.base_path, name);
    PLShaderStage *fragment = plLoadShaderStage(path, PL_SHADER_TYPE_FRAGMENT);
    if(fragment == NULL) {
        Error("failed to load %s, aborting!\n%s", path, plGetError());
    }

    PLShaderProgram *program = plCreateShaderProgram();
    if(program == NULL) {
        Error("failed to create shader program, %s, aborting!\n%s", name, plGetError());
    }

    plAttachShaderStage(program, vertex);
    plAttachShaderStage(program, fragment);
    plLinkShaderProgram(program);

    return program;
}

void InitShaders(void) {
    shader_default = LoadShaderProgram("default");

    /* enable the default shader program */
    plSetShaderProgram(shader_default);
}

void ShutdownShaders(void) {
    plDeleteShaderProgram(shader_default);
}
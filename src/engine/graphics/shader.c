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
#include <PL/platform_graphics.h>

#include "../engine.h"
#include "shader.h"

//#define GLSL(...) #__VA_ARGS__

static PLShaderProgram *programs[MAX_SHADERS];
static ShaderProgram last_program = SHADER_GenericUntextured;  /* for resetting following rebuild */

static PLShaderProgram *CreateShaderProgram(const char *vertex, const char *fragment) {
    if(plIsEmptyString(vertex) || plIsEmptyString(fragment)) {
        Error("Invalid stage for shader program, aborting!\n");
    }

    PLShaderProgram *program = plCreateShaderProgram();
    if(program == NULL) {
        Error("Failed to create shader program, aborting!\n%s", plGetError());
    }

    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "shaders/%s.vert", vertex);
    if(!plRegisterShaderStageFromDisk(program, u_find(path), PL_SHADER_TYPE_VERTEX)) {
        LogWarn("Failed to register vertex stage, \"%s\"!\n", vertex);
        plDestroyShaderProgram(program, true);
        return NULL;
    }

    snprintf(path, sizeof(path), "shaders/%s.frag", fragment);
    if(!plRegisterShaderStageFromDisk(program, u_find(path), PL_SHADER_TYPE_FRAGMENT)) {
        LogWarn("Failed to register fragment stage, \"%s\"!\n", fragment);
        plDestroyShaderProgram(program, true);
        return NULL;
    }

    plLinkShaderProgram(program);

    return program;
}

/**
 * @brief Destroy and rebuild shaders; for modifying in real-time
 */
static void RebuildShaders(void) {
    for(unsigned int i = 0; i < MAX_SHADERS; ++i) {
        if(programs[i] != NULL) {
            plDestroyShaderProgram(programs[i], true);
        }
    }

    programs[SHADER_GenericTextured]    = CreateShaderProgram("generic", "texture");
    programs[SHADER_GenericTexturedLit] = CreateShaderProgram("generic", "lit_texture");
    programs[SHADER_GenericUntextured]  = CreateShaderProgram("generic", "vertex_colour");
    programs[SHADER_Water]              = CreateShaderProgram("generic", "water");
    programs[SHADER_AlphaTest]          = CreateShaderProgram("generic", "alpha_test_texture");
    programs[SHADER_DebugTest]          = CreateShaderProgram("generic", "debug");

    /* set defaults */
    for(unsigned int i = 0; i < MAX_SHADERS; ++i) {
        if(programs[i] == NULL) {
            continue;
        }

        int slot = plGetShaderUniformSlot(programs[i], "diffuse");
        if(slot != -1) {
            plSetShaderUniformInt(programs[i], slot, 0);
        }
    }

    /* switch back to the previous shader program */
    Shaders_SetProgram(last_program);
}

static void RebuildShadersCommand(unsigned int argc, char *argv[]) {
    RebuildShaders();
}

void Shaders_Initialize(void) {
    memset(programs, 0, sizeof(PLShaderProgram*) * MAX_SHADERS);
    RebuildShaders();

    plRegisterConsoleCommand("rebuildShaders", RebuildShadersCommand, "Rebuild all shaders");
}

void Shaders_SetProgram(ShaderProgram program) {
    last_program = program;
    plSetShaderProgram(programs[program]);
}

PLShaderProgram *Shaders_GetProgram(ShaderProgram program) {
    return programs[program];
}

void Shaders_Shutdown(void) {
    for(unsigned int i = 0; i < MAX_SHADERS; ++i) {
        if(programs[i] == NULL) {
            continue;
        }
        plDestroyShaderProgram(programs[i], true);
    }
}

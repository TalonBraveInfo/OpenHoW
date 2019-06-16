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

#define GLSL(...) #__VA_ARGS__
#define GLSL_DEFAULT_PS_UNIFORMS "uniform sampler2D diffuse;"

static const char *fragment_water =
        GLSL_DEFAULT_PS_UNIFORMS
        GLSL(
                void main() {
                    pl_frag = vec4(0.0, 0.0, 1.0, 1.0);
                }
        );

static const char *vertex_water =
        GLSL(
                attribute vec2 position;

                void main() {
                    gl_TexCoord[0] = gl_MultiTexCoord0;
                    gl_Position = ftransform();
                }
        );

static const char *fragment_colour =
        GLSL_DEFAULT_PS_UNIFORMS
        GLSL(
                in vec3 interp_normal;
                in vec2 interp_UV;
                in vec4 interp_colour;

                void main() {
                    pl_frag = interp_colour;
                }
        );

static const char *fragment_texture =
        GLSL_DEFAULT_PS_UNIFORMS
        GLSL(
                in vec3 interp_normal;
                in vec2 interp_UV;
                in vec4 interp_colour;

                void main() {
                    pl_frag = interp_colour * texture(diffuse, interp_UV);
                }
        );

static const char *fragment_alpha_test_texture =
        GLSL_DEFAULT_PS_UNIFORMS
        GLSL(
                in vec3 interp_normal;
                in vec2 interp_UV;
                in vec4 interp_colour;

                void main() {
                    vec4 sample = texture(diffuse, interp_UV);
                    if(sample.a < 0.1) {
                        discard;
                    }

                    pl_frag = interp_colour * sample;
                }
        );

static const char *vertex_default =
        GLSL(
                out vec3 interp_normal;
                out vec2 interp_UV;
                out vec4 interp_colour;

                void main() {
                    gl_Position = pl_proj * pl_view * pl_model * vec4(pl_vposition, 1.0f);
                    interp_normal = pl_vnormal;
                    interp_UV = pl_vuv;
                    interp_colour = pl_vcolour;
                }
        );

static const char *vertex_debug_test =
        GLSL(
                void main() {
                    gl_Position = pl_proj * pl_view * pl_model * vec4(pl_vposition, 1.0f);
                }
        );

static const char *fragment_debug_test =
        GLSL_DEFAULT_PS_UNIFORMS
        GLSL(
                void main() {
                    pl_frag = vec4(1.0f, 0.0f, 1.0f, 1.0f);
                }
        );

PLShaderProgram *programs[MAX_SHADERS];

static PLShaderProgram *CreateShaderProgram(const char *vertex, const char *fragment) {
    if(plIsEmptyString(vertex) || plIsEmptyString(fragment)) {
        Error("Invalid stage for shader program, aborting!\n");
    }

    PLShaderProgram *program = plCreateShaderProgram();
    if(program == NULL) {
        Error("Failed to create shader program, aborting!\n%s", plGetError());
    }

#if 0
    char path[PL_SYSTEM_MAX_PATH];
    snprintf(path, sizeof(path), "shaders/%s.vert", vertex);
    if(!plRegisterShaderStageFromDisk(program, pork_find(path), PL_SHADER_TYPE_VERTEX)) {
        Error("failed to register vertex stage, \"%s\", aborting!\n", vertex);
    }

    snprintf(path, sizeof(path), "shaders/%s.frag", fragment);
    if(!plRegisterShaderStageFromDisk(program, pork_find(path), PL_SHADER_TYPE_FRAGMENT)) {
        Error("failed to register fragment stage, \"%s\", aborting!\n", fragment);
    }
#else
    if(!plRegisterShaderStageFromMemory(program, vertex, strlen(vertex), PL_SHADER_TYPE_VERTEX)) {
        Error("Failed to register vertex stage, \"%s\", aborting!\n", plGetError());
    }

    if(!plRegisterShaderStageFromMemory(program, fragment, strlen(fragment), PL_SHADER_TYPE_FRAGMENT)) {
        Error("Failed to register fragment stage, \"%s\", aborting!\n", plGetError());
    }
#endif

    plLinkShaderProgram(program);

    return program;
}

void Shaders_Initialize(void) {
    memset(programs, 0, sizeof(PLShaderProgram*) * MAX_SHADERS);
    programs[SHADER_DEFAULT]    = CreateShaderProgram(vertex_default, fragment_texture);
    programs[SHADER_UNTEXTURED] = CreateShaderProgram(vertex_default, fragment_colour);
    programs[SHADER_WATER]      = CreateShaderProgram(vertex_default, fragment_water);
    programs[SHADER_ALPHA_TEST] = CreateShaderProgram(vertex_default, fragment_alpha_test_texture);
    programs[SHADER_DEBUG_TEST] = CreateShaderProgram(vertex_debug_test, fragment_debug_test);

    /* set defaults */
    for(unsigned int i = 0; i < MAX_SHADERS; ++i) {
        if(programs[i] == NULL) {
            continue;
        }

        plSetNamedShaderUniformInt(programs[i], "diffuse", 0);
    }

    /* enable the default shader program */
    plSetShaderProgram(programs[SHADER_DEFAULT]);
}

void Shaders_Shutdown(void) {
    for(unsigned int i = 0; i < MAX_SHADERS; ++i) {
        if(programs[i] == NULL) {
            continue;
        }
        plDestroyShaderProgram(programs[i], true);
    }
}

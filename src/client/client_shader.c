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
#include <PL/platform_graphics.h>

#include "../pork_engine.h"

#include "client_shader.h"

#define GLSL(...) #__VA_ARGS__
#define GLSL_DEFAULT_UNIFORMS \
        "uniform sampler2D diffuse;"

static const char *fragment_water =
        GLSL_DEFAULT_UNIFORMS
        GLSL(
                void main() {
                    gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
                }
        );

static const char *vertex_water =
        GLSL_DEFAULT_UNIFORMS
        GLSL(
                attribute vec2 position;

                void main() {
                    gl_TexCoord[0] = gl_MultiTexCoord0;
                    gl_Position = ftransform();
                }
        );

static const char *fragment_default =
        GLSL_DEFAULT_UNIFORMS
        GLSL(
                varying vec4 v_colour;

                void main() {
                    gl_FragColor = v_colour * texture2D(diffuse, gl_TexCoord[0].st);
                }
        );

static const char *fragment_alpha_test =
        GLSL_DEFAULT_UNIFORMS
        GLSL(
                varying vec4 v_colour;

                void main() {
                    vec4 colour = texture2D(diffuse, gl_TexCoord[0].st);
                    if(colour.a < 0.1) {
                        discard;
                    }

                    gl_FragColor = v_colour * colour;
                }
        );

static const char *vertex_default =
        GLSL_DEFAULT_UNIFORMS
        GLSL(
                varying vec4 v_colour;

                void main() {
                    v_colour = gl_Color;

                    gl_TexCoord[0] = gl_MultiTexCoord0;
                    gl_Position = ftransform();
                }
        );

PLShaderProgram *programs[MAX_SHADERS];

PLShaderProgram *CreateShaderProgram(const char *vertex, size_t vl, const char *fragment, size_t fl) {
    if(plIsEmptyString(vertex) || plIsEmptyString(fragment)) {
        Error("invalid stage for shader program, aborting!\n");
    }

    PLShaderProgram *program = plCreateShaderProgram();
    if(program == NULL) {
        Error("failed to create shader program, aborting!\n%s", plGetError());
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
    if(!plRegisterShaderStageFromMemory(program, vertex, vl, PL_SHADER_TYPE_VERTEX)) {
        Error("failed to register vertex stage, \"%s\", aborting!\n", vertex);
    }

    if(!plRegisterShaderStageFromMemory(program, fragment, fl, PL_SHADER_TYPE_FRAGMENT)) {
        Error("failed to register fragment stage, \"%s\", aborting!\n", fragment);
    }
#endif

    plLinkShaderProgram(program);

    plRegisterShaderProgramUniforms(program);

    return program;
}

void InitShaders(void) {
    memset(programs, 0, sizeof(PLShaderProgram*) * MAX_SHADERS);
    programs[SHADER_DEFAULT]    = CreateShaderProgram(vertex_default, strlen(vertex_default),
                                                      fragment_default, strlen(fragment_default));
    programs[SHADER_WATER]      = CreateShaderProgram(vertex_water, strlen(vertex_water),
                                                      fragment_water, strlen(fragment_water));
    programs[SHADER_ALPHA_TEST] = CreateShaderProgram(vertex_default, strlen(vertex_default),
                                                      fragment_alpha_test, strlen(fragment_alpha_test));
    //programs[SHADER_VIDEO]      = CreateShaderProgram("video", "video");

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

void ShutdownShaders(void) {
    for(unsigned int i = 0; i < MAX_SHADERS; ++i) {
        if(programs[i] == NULL) {
            continue;
        }
        plDeleteShaderProgram(programs[i], true);
    }
}

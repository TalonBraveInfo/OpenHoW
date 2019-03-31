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
#define GLSL_DEFAULT_VS_UNIFORMS "attribute vec3 pos; attribute vec3 norm; attribute vec2 UV; attribute vec4 col;"
#define GLSL_DEFAULT_PS_UNIFORMS "uniform sampler2D diffuse;"

static const char *fragment_water =
        GLSL_DEFAULT_PS_UNIFORMS
        GLSL(
                void main() {
                    gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
                }
        );

static const char *vertex_water =
        GLSL_DEFAULT_VS_UNIFORMS
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
                    gl_FragColor = interp_colour;
                }
        );

static const char *fragment_texture =
        GLSL_DEFAULT_PS_UNIFORMS
        GLSL(
                in vec3 interp_normal;
                in vec2 interp_UV;
                in vec4 interp_colour;

                void main() {
                    gl_FragColor = interp_colour * texture2D(diffuse, interp_UV);
                }
        );

static const char *fragment_alpha_test_texture =
        GLSL_DEFAULT_PS_UNIFORMS
        GLSL(
                in vec3 interp_normal;
                in vec2 interp_UV;
                in vec4 interp_colour;

                void main() {
                    vec4 sample = texture2D(diffuse, interp_UV);
                    if(sample.a < 0.1) {
                        discard;
                    }

                    gl_FragColor = interp_colour * sample;
                }
        );

static const char *vertex_default =
        GLSL_DEFAULT_VS_UNIFORMS
        GLSL(
                out vec3 interp_normal;
                out vec2 interp_UV;
                out vec4 interp_colour;

                void main() {
                    gl_Position = pl_proj * pl_model_view * vec4(pos, 1.0f);
                    interp_normal = norm;
                    interp_UV = UV;
                    interp_colour = col;
                }
        );

static const char *vertex_debug_test =
        GLSL_DEFAULT_VS_UNIFORMS
        GLSL(
                void main() {
                    gl_Position = pl_proj * pl_model_view * vec4(pos, 1.0f);
                }
        );

static const char *fragment_debug_test =
        GLSL_DEFAULT_PS_UNIFORMS
        GLSL(
                void main() {
                    gl_FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
                }
        );

PLShaderProgram *programs[MAX_SHADERS];

static PLShaderProgram *CreateShaderProgram(const char *vertex, size_t vl, const char *fragment, size_t fl) {
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
    if(!plRegisterShaderStageFromMemory(program, vertex, vl, PL_SHADER_TYPE_VERTEX)) {
        Error("Failed to register vertex stage, \"%s\", aborting!\n", plGetError());
    }

    if(!plRegisterShaderStageFromMemory(program, fragment, fl, PL_SHADER_TYPE_FRAGMENT)) {
        Error("Failed to register fragment stage, \"%s\", aborting!\n", plGetError());
    }
#endif

    plLinkShaderProgram(program);

    plRegisterShaderProgramUniforms(program);

    return program;
}

void Shaders_Initialize(void) {
    memset(programs, 0, sizeof(PLShaderProgram*) * MAX_SHADERS);
    programs[SHADER_DEFAULT]    = CreateShaderProgram(vertex_default, strlen(vertex_default), fragment_texture, strlen(fragment_texture));
    programs[SHADER_WATER]      = CreateShaderProgram(vertex_default, strlen(vertex_default), fragment_water, strlen(fragment_water));
    programs[SHADER_ALPHA_TEST] = CreateShaderProgram(vertex_default, strlen(vertex_default), fragment_alpha_test_texture, strlen(fragment_alpha_test_texture));
    //programs[SHADER_VIDEO]      = CreateShaderProgram("video", "video");
    programs[SHADER_DEBUG_TEST]       = CreateShaderProgram(vertex_debug_test, strlen(vertex_debug_test), fragment_debug_test, strlen(fragment_debug_test));





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
        plDeleteShaderProgram(programs[i], true);
    }
}

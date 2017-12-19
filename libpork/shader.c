/* OpenHOW
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
#include "engine.h"
#include "engine_gl.h"

typedef struct ShaderProgram {
    GLuint program_id;
} ShaderProgram;

ShaderProgram *gl_programs;
unsigned int num_gl_programs;

enum {
    SHADER_PROGRAM_DEFAULT,
};

void InitShaders(void) {
    num_gl_programs = 12;
    gl_programs = calloc(num_gl_programs, sizeof(ShaderProgram));
    if(gl_programs == NULL) {
        print_error("failed to allocate memory for %d shader programs, aborting!\n");
    }

    gl_programs[SHADER_PROGRAM_DEFAULT] = LoadShaderProgram();
}

void ClearShaders(void) {}

ShaderProgram *LoadShaderProgram(void) {
    ShaderProgram *program = calloc(1, sizeof(ShaderProgram));

}
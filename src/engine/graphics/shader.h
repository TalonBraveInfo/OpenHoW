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

#pragma once

enum {
    SHADER_DEFAULT,     /* unlit */
    SHADER_GOURAUD_LIT, /* lit */
    SHADER_ALPHA_TEST,  /* */
    SHADER_WATER,       /* */
    SHADER_VIDEO,       /* */
    SHADER_DEBUG_TEST,

    MAX_SHADERS
};

PL_EXTERN_C

extern PLShaderProgram *programs[MAX_SHADERS];

void Shaders_Initialize(void);
void Shaders_Shutdown(void);

PL_EXTERN_C_END

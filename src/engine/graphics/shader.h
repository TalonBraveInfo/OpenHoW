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

typedef enum ShaderProgram {
    SHADER_GenericTextured,
    SHADER_GenericUntextured,
    SHADER_GenericTexturedLit,
    SHADER_AlphaTest,
    SHADER_Water,
    SHADER_DebugTest,

    MAX_SHADERS
} ShaderProgram;

PL_EXTERN_C

void Shaders_Initialize(void);
void Shaders_SetProgram(ShaderProgram program);
PLShaderProgram *Shaders_GetProgram(ShaderProgram program);
void Shaders_Shutdown(void);

PL_EXTERN_C_END

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

PL_EXTERN_C

void Script_Initialize(void);
void Script_EvaluateString(const char *str);
void Script_Shutdown(void);

bool Script_LoadFile(const char *path);

void Script_DeclareGlobalString(const char *name, const char *value);
void Script_DeclareGlobalInteger(const char *name, int value);

#define Script_DeclareGlobalStringU(value) \
    Script_DeclareGlobalString(#value, (value))
#define Script_DeclareGlobalIntegerU(value) \
    Script_DeclareGlobalInteger(#value, (value))

/************************************************************/
/* CallScript Functions */

void CF_initGame();
void CF_gameTick();

PL_EXTERN_C_END
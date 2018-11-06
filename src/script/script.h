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

#pragma once

PL_EXTERN_C

void InitScripting(void);
void ShutdownScripting(void);

/**********************************************************/

typedef void ScriptContext;

typedef struct ScriptArray {
    struct {
        char data[64];
    } *strings;
    unsigned int num_strings;
} ScriptArray;

ScriptContext *Script_CreateContext(void);
void Script_DestroyContext(ScriptContext *ctx);

void Script_ParseBuffer(ScriptContext *ctx, const char *buf);
void Script_FlushJSON(ScriptContext *ctx);

unsigned int Script_GetArrayLength(ScriptContext *ctx, const char *property);
ScriptArray *Script_GetArrayStrings(ScriptContext *ctx, const char *property);
void Script_DestroyArrayStrings(ScriptArray *array);
const char *Script_GetArrayObjectString(ScriptContext *ctx, const char *property, uint id, const char *key, const char *def);

const char *Script_GetStringProperty(ScriptContext *ctx, const char *property, const char *def);
int Script_GetIntegerProperty(ScriptContext *ctx, const char *property, int def);

/**********************************************************/
/* CallScript Functions                                   */

void CS_InitGame(void);

PL_EXTERN_C_END
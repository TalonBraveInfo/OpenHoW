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

#include "duktape.h"

PL_EXTERN_C

void InitScripting(void);
void ShutdownScripting(void);

/**********************************************************/

extern duk_context *jsn_context;

void ParseJSON(const char *buf);
void FlushJSON(void);

unsigned int GetJSONArrayLength(const char *property);

const char *GetJSONStringProperty(const char *property);
int GetJSONIntProperty(const char *property);

/**********************************************************/
/* CallScript Functions                                   */

void CS_InitGame(void);

PL_EXTERN_C_END
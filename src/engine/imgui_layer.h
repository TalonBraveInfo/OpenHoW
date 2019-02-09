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

#ifdef __cplusplus

#include <imgui.h>

#else /* C wrapper */

PL_EXTERN_C

void ImGui_NewFrame(void);

bool ImGui_Begin(const char *name, bool *p_open, uint flags);
void ImGui_End(void);
bool ImGui_BeginChild(const char *str_id, PLVector2 size, bool border, uint flags);
void ImGui_EndChild(void);

bool ImGui_SliderFloat(const char *label, float *v, float v_min, float v_max, const char *format, float power);

int ImGui_GetKeyIndex(int imgui_key);

bool ImGui_BeginMainMenuBar(void);
void ImGui_EndMainMenuBar(void);
bool ImGui_BeginMenuBar(void);
void ImGui_EndMenuBar(void);
bool ImGui_BeginMenu(const char *label, bool enabled);
void ImGui_EndMenu(void);
bool ImGui_MenuItem(const char *label, const char *shortcut, bool selected, bool enabled);
bool ImGui_MenuItemToggle(const char *label, const char *shortcut, bool *p_selected, bool enabled);

PL_EXTERN_C_END

#endif

/************************************************************/

PL_EXTERN_C

void UI_DisplayDebugMenu(void);

PL_EXTERN_C_END

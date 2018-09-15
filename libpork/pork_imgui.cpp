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

#include "pork_engine.h"
#include "pork_imgui.h"

/****************************************************/
/* C Wrapper */

/* main ... */

void ImGui_NewFrame(void) {
    ImGui::NewFrame();
}

/* windows ... */

bool ImGui_Begin(const char *name, bool *p_open, uint flags) {
    return ImGui::Begin(name, p_open, flags);
}

void ImGui_End(void) {
    ImGui::End();
}

bool ImGui_BeginChild(const char *str_id, PLVector2 size, bool border, uint flags) {
    return ImGui::BeginChild(str_id, ImVec2(size.x, size.y), border, flags);
}

void ImGui_EndChild(void) {
    ImGui::EndChild();
}

/* sliders ... */

bool ImGui_SliderFloat(const char *label, float *v, float v_min, float v_max, const char *format, float power) {
    return ImGui::SliderFloat(label, v, v_min, v_max, format, power);
}

bool ImGui_SliderAngle(const char *label, float *v_rad, float v_degrees_min, float v_degrees_max) {
    return ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max);
}

bool ImGui_SliderInt(const char *label, int *v, int v_min, int v_max, const char *format) {
    return ImGui::SliderInt(label, v, v_min, v_max, format);
}

/* inputs ... */

int ImGui_GetKeyIndex(int imgui_key) {
    return ImGui::GetKeyIndex(imgui_key);
}

/* menus ... */

bool ImGui_BeginMainMenuBar(void) {
    return ImGui::BeginMainMenuBar();
}

void ImGui_EndMainMenuBar(void) {
    ImGui::EndMainMenuBar();
}

bool ImGui_BeginMenuBar(void) {
    return ImGui::BeginMenuBar();
}

void ImGui_EndMenuBar(void) {
    ImGui::EndMenuBar();
}

bool ImGui_BeginMenu(const char *label, bool enabled) {
    return ImGui::BeginMenu(label, enabled);
}

void ImGui_EndMenu(void) {
    ImGui::EndMenu();
}

bool ImGui_MenuItem(const char *label, const char *shortcut, bool selected, bool enabled) {
    return ImGui::MenuItem(label, shortcut, selected, enabled);
}

bool ImGui_MenuItemToggle(const char *label, const char *shortcut, bool *p_selected, bool enabled) {
    return ImGui::MenuItem(label, shortcut, p_selected, enabled);
}

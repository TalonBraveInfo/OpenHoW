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

#include "client/client_display.h"

static bool show_quit = false;
static bool show_file = false;
static bool show_about = false;
static bool show_console = false;
static bool show_fps = false;

void UI_DisplayFileBox(void) {
    if(show_file) {
        ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
        ImGui::Begin("About", &show_file, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize |
                                          ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::End();
    }
}

void UI_DisplayQuitBox(void) {
    if(show_quit) {
        ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
        ImGui::Begin("Are you sure?", &show_quit, ImGuiWindowFlags_Modal | ImGuiWindowFlags_NoResize |
                                                  ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Are you sure you want to quit the game?\nAny unsaved changes will be lost!\n");
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 5));

        if (ImGui::Button("Yes", ImVec2(64, 0))) {
            System_Shutdown();
        }

        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(64, 0))) {
            show_quit = false;
        }

        ImGui::End();
    }
}

void UI_DisplayConsole(void) {

}

void UI_DisplayDebugMenu(void) {
    /* keep vars in sync with console vars, in case they
     * are changed during startup or whenever... */

    show_fps = cv_debug_fps->b_value;

    /* */

#if 0
    ImGui::Begin("Game", NULL);
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    UpdateViewport(pos.x, pos.y, size.x, size.y);
    ImGui::End();
#endif

    if(ImGui::BeginMainMenuBar()) {

        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Open...", "CTRL+O")) {

            }

            ImGui::Separator();
            ImGui::MenuItem("Quit", "CTRL+Q", &show_quit);
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Debug")) {
            ImGui::MenuItem("Show Console", "`", &show_console);
            if(ImGui::MenuItem("Show FPS", NULL, &show_fps)) {
                plSetConsoleVariable(cv_debug_fps, show_fps ? "true" : "false");
            }

            static int tc = 0;
            if(ImGui::SliderInt("Show Texture Cache", &tc, 0, MAX_TEXTURE_INDEX)) {
                char buf[4];
                plSetConsoleVariable(cv_display_texture_cache, pl_itoa(tc - 1, buf, 4, 10));
            }

            static int im = 0;
            if(ImGui::SliderInt("Show Input States", &im, 0, 2)) {
                char buf[4];
                plSetConsoleVariable(cv_debug_input, pl_itoa(im, buf, 4, 10));
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About", NULL, &show_about);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    UI_DisplayQuitBox();
}

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

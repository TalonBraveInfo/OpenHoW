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

#include <PL/platform_graphics_camera.h>
#include <PL/platform_filesystem.h>

#include "engine.h"
#include "imgui_layer.h"

#include "client/display.h"

static bool show_quit               = false;
static bool show_file               = false;
static bool show_about              = false;
static bool show_console            = false;
static bool show_texture            = false;
static bool show_fps                = false;

class BaseWindow {
public:
    virtual void ShowWindow(bool show = true) { show_ = show; }
    bool IsWindowShowing() { return show_; }

protected:
private:
    bool show_{false};
};

/****************************************************/
/* Particle Editor */

#include "client/particle.h"

class ParticleEditor {
public:
    static ParticleEditor *GetInstance() {
        static ParticleEditor *instance = nullptr;
        if(instance == nullptr) {
            instance = new ParticleEditor();
        }

        return instance;
    }

    void ShowWindow(bool show = true) { show_ = show; }
    void DisplayWindow() {
        if(!show_) {
            return;
        }
    }

protected:
private:
    ParticleSystem *current_system_{nullptr};

    bool show_{false};
};

/****************************************************/
/* Texture Viewer */

static PLTexture *cur_texture = nullptr;

void UI_DisplayTextureViewer() {
    if(!show_texture) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(cur_texture->w, cur_texture->h), ImGuiCond_Once);
    ImGui::Begin("Texture Viewer", &show_texture, ImGuiWindowFlags_MenuBar);

    if(ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Open...")) {
                show_file = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Image(reinterpret_cast<ImTextureID>(cur_texture->internal.id), ImVec2(cur_texture->w, cur_texture->h));
    ImGui::Separator();
    ImGui::Text("Name: %s", cur_texture->name);
    ImGui::Text("W:%d H:%d", cur_texture->w, cur_texture->h);
    ImGui::Text("Size: %lu", cur_texture->size);

    ImGui::End();
}

/****************************************************/

enum {
    FILE_TYPE_UNKNOWN,
    FILE_TYPE_MAP,
    FILE_TYPE_MAP_POG,
    FILE_TYPE_IMAGE,
    FILE_TYPE_AUDIO,
    FILE_TYPE_PARTICLE,

    MAX_FILE_TYPES
};

typedef struct FileDescriptor {
    char path[PL_SYSTEM_MAX_PATH];
    unsigned int type;
} FileDescriptor;

static std::vector<FileDescriptor> file_list;

void AddFilePath(const char *path) {
    FileDescriptor descriptor;
    strncpy(descriptor.path, path, sizeof(descriptor.path));
    descriptor.type = FILE_TYPE_UNKNOWN;

    const char *ext = plGetFileExtension(path);
    u_assert(!plIsEmptyString(ext), "failed to identify file, extension was empty!\n");
    if(pl_strncasecmp(ext, "map", 3) == 0) {
        descriptor.type = FILE_TYPE_MAP;
    } else if(
            pl_strncasecmp(ext, "tim", 3) == 0 ||
            pl_strncasecmp(ext, "bmp", 3) == 0 ||
            pl_strncasecmp(ext, "png", 3) == 0
            ) {
        descriptor.type = FILE_TYPE_IMAGE;
    } else if(
            pl_strncasecmp(ext, "pps", 3) == 0
            ) {
        descriptor.type = FILE_TYPE_PARTICLE;
    } else if(
            pl_strncasecmp(ext, "wav", 3) == 0
            ) {
        descriptor.type = FILE_TYPE_AUDIO;
    }

    file_list.push_back(descriptor);
}

void ScanDirectories() {
    file_list.empty();
    plScanDirectory(GetBasePath(), "map", AddFilePath, true);
    plScanDirectory(GetBasePath(), "pog", AddFilePath, true);
    plScanDirectory(GetBasePath(), "pps", AddFilePath, true);
    plScanDirectory(GetBasePath(), "tim", AddFilePath, true);
    plScanDirectory(GetBasePath(), "bmp", AddFilePath, true);
    plScanDirectory(GetBasePath(), "png", AddFilePath, true);
    plScanDirectory(GetBasePath(), "wav", AddFilePath, true);
}

void UI_DisplayFileBox() {
    if(!show_file) {
        return;
    }

    static bool has_scanned = false;
    if(!has_scanned) {
        ScanDirectories();
        has_scanned = true;
    }

    ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiCond_Once);
    ImGui::Begin("Open File", &show_file);

    static ImGuiTextFilter filter;
    filter.Draw();

    if(ImGui::Button("Rescan", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
        ScanDirectories();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(-1.f, 0));
    ImGui::BeginChild("Child2", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight() - 64), true);
    ImGui::Columns(2);
    for (unsigned int i = 0; i < file_list.size(); ++i) {
        if (filter.PassFilter(file_list[i].path)) {
            if(ImGui::Button(file_list[i].path)) {
                switch(file_list[i].type) {
                    case FILE_TYPE_IMAGE: {
                        if(cur_texture != nullptr) {
                            plDeleteTexture(cur_texture, true);
                        }

                        cur_texture = Display_LoadTexture(file_list[i].path, PL_TEXTURE_FILTER_LINEAR);
                        show_texture = true;
                    } break;

                    default:break;
                }
            }

            ImGui::NextColumn();

            const char *type = "Unknown";
            switch(file_list[i].type) {
                case FILE_TYPE_AUDIO:       type = "Audio"; break;
                case FILE_TYPE_PARTICLE:    type = "Particle System"; break;
                case FILE_TYPE_IMAGE:       type = "Image"; break;
                case FILE_TYPE_MAP:         type = "Map"; break;
                case FILE_TYPE_MAP_POG:     type = "Map Objects"; break;

                default:break;
            }

            ImGui::Text("%s", type);

            ImGui::NextColumn();
        }
    }
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    ImGui::End();
}

void UI_DisplayQuitBox() {
    if(!show_quit) {
        return;
    }

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

void UI_DisplayConsole() {
    if(!show_console) {
        return;
    }

    static char buf[256];

    ImGui::SetNextWindowSize(ImVec2(Display_GetViewportWidth(&g_state.camera->viewport) - 20, 128), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(10, Display_GetViewportHeight(&g_state.camera->viewport) - 138));
    ImGui::Begin("Console", &show_console);
    if(ImGui::InputText("Input", buf, 256)) {

    }
    ImGui::End();
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
            if(ImGui::MenuItem("Open...")) { show_file = true; }
            ImGui::Separator();
            if(ImGui::MenuItem("Quit")) { show_quit = true; }
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

            if(ImGui::IsItemHovered() && tc > 0) {
                const PLTexture *texture = Display_GetCachedTexture((unsigned int) cv_display_texture_cache->i_value);
                if(texture != NULL) {
                    ImGui::BeginTooltip();
                    ImGui::Image(reinterpret_cast<ImTextureID>(texture->internal.id), ImVec2(texture->w, texture->h));
                    ImGui::Text("%d (%dx%d)", cv_display_texture_cache->i_value, texture->w, texture->h);
                    ImGui::EndTooltip();
                }
            }

            static int im = 0;
            if(ImGui::SliderInt("Show Input States", &im, 0, 2)) {
                char buf[4];
                plSetConsoleVariable(cv_debug_input, pl_itoa(im, buf, 4, 10));
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Tools")) {
            if(ImGui::MenuItem("Particle Editor...")) {
                ParticleEditor::GetInstance()->ShowWindow(true);
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About", NULL, &show_about);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    UI_DisplayFileBox();
    UI_DisplayQuitBox();

    /* Editors */
    ParticleEditor::GetInstance()->DisplayWindow();
    UI_DisplayTextureViewer();

    UI_DisplayConsole();
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

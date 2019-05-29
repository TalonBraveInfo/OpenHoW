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
#include "../3rdparty/imgui/examples/imgui_impl_opengl3.h"
#include "MapManager.h"
#include "audio.h"

#include "graphics/display.h"

static PLCamera *imgui_camera = nullptr;

void ImGuiImpl_SetupCamera(void) {
    if((imgui_camera = plCreateCamera()) == nullptr) {
        Error("failed to create ui camera, aborting!\n%s\n", plGetError());
    }

    imgui_camera->mode         = PL_CAMERA_MODE_ORTHOGRAPHIC;
    imgui_camera->fov          = 90;
    imgui_camera->near         = 0;
    imgui_camera->far          = 1000;
    imgui_camera->viewport.w   = cv_display_width->i_value;
    imgui_camera->viewport.h   = cv_display_height->i_value;
}

void ImGuiImpl_SetupFrame(void) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiImpl_UpdateViewport(int w, int h) {
    imgui_camera->viewport.w = w;
    imgui_camera->viewport.h = h;
}

void ImGuiImpl_Draw(void) {
    ImGui::Render();

    plSetupCamera(imgui_camera);
    plSetShaderProgram(nullptr);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/************************************************************/

static bool show_quit               = false;
static bool show_file               = false;
static bool show_new_game           = false;
static bool show_console            = false;
static bool show_settings           = false;

static unsigned int windows_id_counter = 0;
class DebugWindow {
public:
    explicit DebugWindow(DebugWindow *parent = nullptr, bool status = true) {
        id_     = windows_id_counter++;
        status_ = status;
        parent_ = parent;
    }

    virtual ~DebugWindow() = default;

    virtual void Display() = 0;

    bool GetStatus() { return status_; }
    void SetStatus(bool status) { status_ = status; }

    void ToggleStatus() {
        status_ = !status_;
    }

protected:
    bool status_;
    unsigned int id_;

    DebugWindow *parent_{nullptr};
};

#define dname(a)  std::string(a "##" + /* std::to_string((ptrdiff_t)(this))*/ std::to_string(id_)).c_str()

static const unsigned int default_flags = ImGuiWindowFlags_NoSavedSettings;

static std::vector<DebugWindow*> windows;

/************************************************************/
/* Map Config Editor */

class MapConfigEditor : public DebugWindow {
public:
    MapConfigEditor();
    explicit MapConfigEditor(const std::string &path);

    ~MapConfigEditor() override;

    void Display() override;

    void OpenManifest(const std::string &path);
    void SyncManifest();
    void CloseManifest();
    void SaveManifest(const std::string &path);

protected:
private:
    MapManifest manifest_;

    bool save_dialog{false};

    char name_buffer[32]{'\0'};
    char author_buffer[32]{'\0'};
    char sky_buffer[32]{'\0'};
    char filename_buffer[32]{'\0'};
};

MapConfigEditor::MapConfigEditor() = default;
MapConfigEditor::MapConfigEditor(const std::string &path) {
    OpenManifest(path);
}

MapConfigEditor::~MapConfigEditor() = default;

void MapConfigEditor::Display() {
    ImGui::SetNextWindowSize(ImVec2(256, 512), ImGuiCond_Once);

    ImGui::Begin(dname("Map Config Editor"), &status_,
                 ImGuiWindowFlags_MenuBar | default_flags);

    if(ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("New")) {
                CloseManifest();
            }

            ImGui::Separator();

            if(ImGui::MenuItem("Save")) {
                if(filename_buffer[0] == '\0') {
                    save_dialog = true;
                }
            }
            if(ImGui::MenuItem("Save As...")) {
                save_dialog = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::InputText("Name", name_buffer, sizeof(name_buffer));
    ImGui::InputText("Author", author_buffer, sizeof(author_buffer));

    // todo: mode selection - need to query wherever this ends up being implemented...

    ImGui::Separator();

    ImGui::InputText("Sky", sky_buffer, sizeof(sky_buffer));

    ImGui::SliderAngle("Sun Pitch", &manifest_.sun_pitch, 0, 90, nullptr);
    ImGui::SliderAngle("Sun Yaw", &manifest_.sun_yaw, 0, 360, nullptr);

    float rgb[3];
    rgb[0] = plByteToFloat(manifest_.sun_colour.r);
    rgb[1] = plByteToFloat(manifest_.sun_colour.g);
    rgb[2] = plByteToFloat(manifest_.sun_colour.b);
    ImGui::ColorPicker3("Sun Colour", rgb, ImGuiColorEditFlags_InputRGB);
    manifest_.sun_colour.r = plFloatToByte(rgb[0]);
    manifest_.sun_colour.g = plFloatToByte(rgb[1]);
    manifest_.sun_colour.b = plFloatToByte(rgb[2]);

    rgb[0] = plByteToFloat(manifest_.ambient_colour.r);
    rgb[1] = plByteToFloat(manifest_.ambient_colour.g);
    rgb[2] = plByteToFloat(manifest_.ambient_colour.b);
    ImGui::ColorPicker3("Ambient Colour", rgb, ImGuiColorEditFlags_InputRGB);
    manifest_.ambient_colour.r = plFloatToByte(rgb[0]);
    manifest_.ambient_colour.g = plFloatToByte(rgb[1]);
    manifest_.ambient_colour.b = plFloatToByte(rgb[2]);

    ImGui::Separator();

    const char *temperatures[]={ "hot", "cold" };

    //ImGui::ListBox("Temperature", temperatures, , );

    ImGui::End();

    if(save_dialog) {
        ImGui::SetNextWindowSize(ImVec2(256, 128), ImGuiCond_Once);
        ImGui::Begin(dname("Map Config Editor / Save"), &save_dialog, default_flags);
        ImGui::InputText("Name", filename_buffer, sizeof(filename_buffer));
        ImGui::Separator();
        if(ImGui::Button("Save")) {
            SaveManifest(std::string(GetFullCampaignPath()) + "/maps/" + filename_buffer + ".map");
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")) {
            save_dialog = false;
        }
        ImGui::End();
    }
}

void MapConfigEditor::OpenManifest(const std::string &path) {
    std::string name = plGetFileName(path.c_str());
    name.erase(name.find(".map"));
    manifest_ = *MapManager::GetInstance()->GetManifest(name);

    SyncManifest();

    snprintf(filename_buffer, sizeof(filename_buffer), "%s", path.c_str());
}

void MapConfigEditor::CloseManifest() {
    manifest_ = MapManifest();
    SyncManifest();
}

void MapConfigEditor::SyncManifest() {
    strncpy(name_buffer, manifest_.name.c_str(), sizeof(name_buffer));
    strncpy(author_buffer, manifest_.author.c_str(), sizeof(author_buffer));
    strncpy(sky_buffer, manifest_.sky.c_str(), sizeof(sky_buffer));
}

void MapConfigEditor::SaveManifest(const std::string &path) {
    std::ofstream output(path);
    if(!output.is_open()) {
        LogWarn("Failed to write to \"%s\", aborting!n\"\n", filename_buffer);
        return;
    }

    output << "{";
    output << R"("name":")" + manifest_.name + "\",";
    output << R"("author":")" + manifest_.author + "\",";
    output << R"("description":")" + manifest_.description + "\",";
    output << R"("sky":")" + manifest_.sky + "\",";
    if(!manifest_.modes.empty()) {
        output << R"("modes":[)";
        for (unsigned int i = 0; i < manifest_.modes.size(); ++i) {
            output << "\"" + manifest_.modes[i] + "\"";
            if (i != manifest_.modes.size()) {
                output << ",";
            }
        }
        output << "],";
    }
    output << R"("ambientColour":")" +
        std::to_string(manifest_.ambient_colour.r) + " " +
        std::to_string(manifest_.ambient_colour.g) + " " +
        std::to_string(manifest_.ambient_colour.b) + "\",";
    output << R"("sunColour":")" +
        std::to_string(manifest_.sun_colour.r) + " " +
        std::to_string(manifest_.sun_colour.g) + " " +
        std::to_string(manifest_.sun_colour.b) + "\",";
    output << R"("sunYaw":")" + std::to_string(manifest_.sun_yaw) + "\",";
    output << R"("sunPitch":")" + std::to_string(manifest_.sun_pitch) + "\",";
    output << R"("temperature":")" + manifest_.temperature + "\",";
    output << R"("time":")" + manifest_.time + "\"";
    output << "}\n";

    LogInfo("Wrote \"%s\"!\n", path.c_str());
}

/************************************************************/

#include "TextureViewer.h"
#include "game/TempGame.h"
#include "game/Game.h"

/************************************************************/
/* Settings */

void UI_DisplaySettings() {
    if(!show_settings) {
        return;
    }

    ImGui::Begin("Settings", &show_settings);
    
    static int item_current = 0;
    bool display_changed = false;
    const VideoPreset* mode = Display_GetVideoPreset(item_current);
    char s[32] = { 0 };
    snprintf(s, 32, "%dx%d", mode->width, mode->height);
    if (ImGui::BeginCombo("Resolution", s, 0))
    {
        int num_modes = Display_GetNumVideoPresets();
        for (int n = 0; n < num_modes; n++)
        {
            mode = Display_GetVideoPreset(n);
            snprintf(s, 32, "%dx%d", mode->width, mode->height);
            bool is_selected = (item_current == n);
            if (ImGui::Selectable(s, is_selected)){
                item_current = n;
                display_changed = true;
                break;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    static bool fs;
    bool fs_changed = false;
    fs_changed |= ImGui::Checkbox("Fullscreen", &fs);

    static int aspect_opt = 0;
    bool aspect_changed = false;
    ImGui::Text("Aspect ratio:");
    aspect_changed |= ImGui::RadioButton("4:3", &aspect_opt, 0); ImGui::SameLine();
    aspect_changed |= ImGui::RadioButton("Fit window", &aspect_opt, 1);

    static int ui_scale_opt = 0;
    bool ui_scale_changed = false;
    ImGui::Text("UI scale:");
    ui_scale_changed |= ImGui::RadioButton("Auto", &ui_scale_opt, 0); ImGui::SameLine();
    ui_scale_changed |= ImGui::RadioButton("1x", &ui_scale_opt, 1); ImGui::SameLine();
    ui_scale_changed |= ImGui::RadioButton("2x", &ui_scale_opt, 2); ImGui::SameLine();
    ui_scale_changed |= ImGui::RadioButton("3x", &ui_scale_opt, 3); ImGui::SameLine();
    ui_scale_changed |= ImGui::RadioButton("4x", &ui_scale_opt, 4);

    //Apply changes
    char buf[16];
    if(display_changed) {
        plSetConsoleVariable(cv_display_width, pl_itoa(mode->width, buf, 16, 10));
        plSetConsoleVariable(cv_display_height, pl_itoa(mode->height, buf, 16, 10));
        System_SetWindowSize(cv_display_width->i_value, cv_display_height->i_value, cv_display_fullscreen->b_value);
    }
    if(fs_changed) {
        plSetConsoleVariable(cv_display_fullscreen, fs ? "true" : "false" );
        System_SetWindowSize(cv_display_width->i_value, cv_display_height->i_value, cv_display_fullscreen->b_value);
    }
    if(aspect_changed) {
        plSetConsoleVariable(cv_display_use_window_aspect, aspect_opt ? "true" : "false");
        Display_UpdateViewport(0, 0, cv_display_width->i_value, cv_display_height->i_value);
    }
    if(ui_scale_changed) {
        plSetConsoleVariable(cv_display_ui_scale, pl_itoa(ui_scale_opt, buf, 16, 10));
    }
    
    if(ImGui::Button("Cancel")) {
        show_settings = false;
    }

    ImGui::End();
}

/************************************************************/
/* New Game */

void UI_DisplayNewGame() {
    if(!show_new_game) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(plGetCurrentViewport()->w, plGetCurrentViewport()->h));
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Select Team", &show_new_game,
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoDecoration | default_flags
    );

#if 0
    ImGui::ListBoxHeader("Maps");
    ImGui::ListBoxFooter();
#endif

#if 0 // todo
    ImGui::SliderInt("Number of Players", reinterpret_cast<int *>(&mode.num_players), 1, 4);

    const char *teams[MAX_TEAMS]={
            /* todo: use translation */
            "Tommy's Trotters",
            "Uncle Hams Hogs",
            "Garlic Grunts",
            "Sow-A-Krauts",
            "Piggystroika",
            "Sushi-Swine",
            "Lard",
    };
    for (unsigned int i = 0; i < mode.num_players; ++i) {
        std::string label_name = "Player " + std::to_string(i);
        ImGui::Combo(label_name.c_str(), reinterpret_cast<int *>(&mode.teams[i]), teams, IM_ARRAYSIZE(teams), 12);
    }

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 32);

    if(ImGui::Button("Start Game!")) {
        mode.force_start = true;
        mode.num_players = 1;
        snprintf(mode.map, sizeof(mode.map) - 1, "camp");
        Game_SetMode(&mode);
    }
    ImGui::SameLine();
    if(ImGui::Button("Cancel")) {
        show_new_game = false;
    }
#else

    if(ImGui::Button("Start Game!")) {

    }

#endif

    ImGui::End();
}

/************************************************************/

enum {
    FILE_TYPE_UNKNOWN,
    FILE_TYPE_MAP,
    FILE_TYPE_MAP_POG,
    FILE_TYPE_MAP_PTG,
    FILE_TYPE_MAP_PMG,
    FILE_TYPE_IMAGE,
    FILE_TYPE_AUDIO,
    FILE_TYPE_PARTICLE,
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
    if(ext != nullptr) {
        if(strcmp(ext, "map") == 0) {
            descriptor.type = FILE_TYPE_MAP;
        } else if(
                strcmp(ext, "tim") == 0 ||
                strcmp(ext, "bmp") == 0 ||
                strcmp(ext, "png") == 0
                ) {
            descriptor.type = FILE_TYPE_IMAGE;
        } else if(
                strcmp(ext, "pps") == 0
                ) {
            descriptor.type = FILE_TYPE_PARTICLE;
        } else if(
                strcmp(ext, "wav") == 0
                ) {
            descriptor.type = FILE_TYPE_AUDIO;
        } else if(
                strcmp(ext, "ptg") == 0
                ) {
            descriptor.type = FILE_TYPE_MAP_PTG;
        } else if(
                strcmp(ext, "pog") == 0
                ) {
            descriptor.type = FILE_TYPE_MAP_POG;
        } else if(
                strcmp(ext, "pmg") == 0
                ) {
            descriptor.type = FILE_TYPE_MAP_PMG;
        }
    }

    file_list.push_back(descriptor);
}

void ScanDirectories() {
    file_list.clear();
    plScanDirectory(GetBasePath(), nullptr, AddFilePath, true);
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
    for (auto &i : file_list) {
        if (filter.PassFilter(i.path)) {
            if(ImGui::Button(i.path)) {
                switch(i.type) {
                    case FILE_TYPE_IMAGE: {
                        PLTexture *texture = Display_LoadTexture(i.path, PL_TEXTURE_FILTER_LINEAR);
                        if(texture == nullptr) {
                            LogWarn("Failed to load specified texture, \"%s\"!\n", i.path);
                            break;
                        }

                        windows.push_back(new TextureViewer(i.path, texture));
                    } break;

                    case FILE_TYPE_MAP: {
                        windows.push_back(new MapConfigEditor(i.path));
                    } break;

                    case FILE_TYPE_AUDIO: {
                        AudioManager::GetInstance()->PlayGlobalSound(i.path);
                    } break;

                    default:break;
                }
            }

            ImGui::NextColumn();

            const char *type = "Unknown";
            switch(i.type) {
                case FILE_TYPE_AUDIO:       type = "Audio"; break;
                case FILE_TYPE_PARTICLE:    type = "Particle System"; break;
                case FILE_TYPE_IMAGE:       type = "Image"; break;
                case FILE_TYPE_MAP:         type = "Map Manifest"; break;
                case FILE_TYPE_MAP_POG:     type = "Map Objects"; break;
                case FILE_TYPE_MAP_PTG:     type = "Map Textures"; break;
                case FILE_TYPE_MAP_PMG:     type = "Map Geometry"; break;
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

class QuitWindow : public DebugWindow {
public:
    void Display() override {
        ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
        ImGui::Begin("Are you sure?", &show_quit, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
                                                  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
        ImGui::Text("Are you sure you want to quit the game?\nAny unsaved changes will be lost!\n");
        ImGui::Dummy(ImVec2(0, 5));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 5));

        if (ImGui::Button("Yes", ImVec2(64, 0))) {
            System_Shutdown();
        }

        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(64, 0))) {
            ToggleStatus();
        }

        ImGui::End();
    }

protected:
private:
};

class ConsoleWindow : public DebugWindow {
public:
    void SendCommand() {
        plParseConsoleString(input_buf_);
        strncpy(input_buf_, "", sizeof(input_buf_));
    }

    void Display() override {
        ImGui::SetNextWindowSize(ImVec2(Display_GetViewportWidth(&g_state.camera->viewport) - 20, 128), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(10, Display_GetViewportHeight(&g_state.camera->viewport) - 138));
        ImGui::Begin("Console", &show_console);
        if(ImGui::InputText("", input_buf_, 256, ImGuiInputTextFlags_EnterReturnsTrue) && input_buf_[0] != '\0') {
            SendCommand();
        }
        ImGui::SameLine();
        if(ImGui::Button("Submit")) {
            SendCommand();
        }
        ImGui::End();
    }

protected:
private:
    char input_buf_[256]{'\0'};
};

void UI_DisplayDebugMenu(void) {
    static bool show_about = false;
    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("New Game...")) {
                show_new_game = true;
            }
            if(ImGui::MenuItem("Load Game...")) {}
            ImGui::Separator();
            if(ImGui::MenuItem("Open...")) { show_file = true; }
            ImGui::Separator();
            if(ImGui::MenuItem("Settings...")) { show_settings = true; }
            ImGui::Separator();
            if(ImGui::MenuItem("Quit")) {
                windows.push_back(new QuitWindow());
            }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Debug")) {
            if(ImGui::MenuItem("Show Console", "`")) {
                windows.push_back(new ConsoleWindow());
            }

            static int tc = 0;
            if(ImGui::SliderInt("Show Texture Cache", &tc, 0, MAX_TEXTURE_INDEX)) {
                char buf[4];
                plSetConsoleVariable(cv_display_texture_cache, pl_itoa(tc - 1, buf, 4, 10));
            }

            if(ImGui::IsItemHovered() && tc > 0) {
                const PLTexture *texture = Display_GetCachedTexture((unsigned int) cv_display_texture_cache->i_value);
                if(texture != nullptr) {
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

            ImGui::Separator();

            if(ImGui::BeginMenu("Console Variables")) {
                size_t num_c;
                PLConsoleVariable **vars;
                plGetConsoleVariables(&vars, &num_c);

                for (PLConsoleVariable **var = vars; var < num_c + vars; ++var) {
                    switch ((*var)->type) {
                        case pl_float_var:
                            if(ImGui::InputFloat((*var)->var, &(*var)->f_value, 0, 10, nullptr,
                                    ImGuiInputTextFlags_EnterReturnsTrue)) {
                                plSetConsoleVariable((*var), std::to_string((*var)->f_value).c_str());
                            }
                            break;
                        case pl_int_var:
                            if (ImGui::InputInt((*var)->var, &(*var)->i_value, 1, 10,
                                                ImGuiInputTextFlags_EnterReturnsTrue)) {
                                plSetConsoleVariable((*var), std::to_string((*var)->i_value).c_str());
                            }
                            break;
                        case pl_string_var:
                            // read-only for now
                            ImGui::LabelText((*var)->var, "%s", (*var)->s_value);
                            break;
                        case pl_bool_var:
                            bool b = (*var)->b_value;
                            if (ImGui::Checkbox((*var)->var, &b)) {
                                plSetConsoleVariable((*var), b ? "true" : "false");
                            }
                            break;
                    }

                    if (ImGui::IsItemHovered()) {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted((*var)->description);
                        ImGui::EndTooltip();
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Tools")) {
            if(ImGui::MenuItem("Map Config Editor...")) {
                windows.push_back(new MapConfigEditor());
            }
            if(ImGui::MenuItem("Particle Editor...")) {}
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About OpenHoW", nullptr, &show_about);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        unsigned int fps, ms;
        Display_GetFramesCount(&fps, &ms);

        ImGui::PushItemWidth(64);
        ImGui::Text("FPS %d (%dms)", fps, ms);
        ImGui::PopItemWidth();

        ImGui::EndMainMenuBar();
    }

    UI_DisplayFileBox();
    UI_DisplayNewGame();
    UI_DisplaySettings();

    for(auto window = windows.begin(); window != windows.end();) {
        if((*window)->GetStatus()) {
            (*window)->Display();
            ++window;
            continue;
        }

        delete (*window);
        window = windows.erase(window);
    }
}

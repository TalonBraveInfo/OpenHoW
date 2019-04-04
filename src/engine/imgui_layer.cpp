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
#include "MapManager.h"
#include "client/audio.h"

static bool show_quit               = false;
static bool show_file               = false;
static bool show_new_game           = false;
static bool show_about              = false;
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

static std::vector<DebugWindow*> windows;

/************************************************************/
/* Map Config Editor */

class MapConfigOpenWindow;

class MapConfigEditor : public DebugWindow {
public:
    MapConfigEditor();
    ~MapConfigEditor() override;

    void Display() override;

    void OpenManifest(const std::string &name);
    void CloseManifest();
    void SaveManifest();

protected:
private:
    MapManifest manifest_;
    MapConfigOpenWindow *open_window_{nullptr};
};

class MapConfigOpenWindow : public DebugWindow {
public:
    explicit MapConfigOpenWindow(DebugWindow *parent) : DebugWindow(parent, false) {
        manifests_ = &MapManager::GetInstance()->GetManifests();
    }

    void Display() override {
        ImGui::SetNextWindowSize(ImVec2(128, 256));
        ImGui::Begin(std::string("Open Map Config##" + std::to_string((ptrdiff_t)(this))).c_str(), &status_);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(-1.f, 0));
        ImGui::BeginChild("Child2", ImVec2(ImGui::GetWindowContentRegionWidth(), ImGui::GetWindowHeight() - 64), true);
        ImGui::Columns(1);
        for (const auto& manifest : *manifests_) {
            if(ImGui::Button(manifest.first.c_str())) {
                status_ = false;

                auto *editor = dynamic_cast<MapConfigEditor*>(parent_);
                editor->OpenManifest(manifest.first);
            }
            //ImGui::NextColumn();
            //ImGui::Text("%s", manifest.second.author.c_str());
            ImGui::NextColumn();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::End();
    }

protected:
private:
    const std::map<std::string, MapManifest> *manifests_;
};

MapConfigEditor::MapConfigEditor() {
    open_window_ = new MapConfigOpenWindow(this);
}

MapConfigEditor::~MapConfigEditor() {
    delete open_window_;
}

void MapConfigEditor::Display() {
    ImGui::SetNextWindowSize(ImVec2(256, 512));

    ImGui::Begin(std::string("Map Config Editor##" + std::to_string((ptrdiff_t)(this))).c_str(), &status_,
                 ImGuiWindowFlags_MenuBar);
    if(ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("New")) {
                open_window_->SetStatus(false);

                manifest_ = MapManifest();
            }
            if(ImGui::MenuItem("Open...")) {
                open_window_->ToggleStatus();
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Save")) {

            }
            if(ImGui::MenuItem("Save As...")) {

            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();

    if(open_window_->GetStatus()) {
        open_window_->Display();
    }
}

void MapConfigEditor::OpenManifest(const std::string &name) {
    manifest_ = *MapManager::GetInstance()->GetManifest(name);
}

/************************************************************/

class TextureViewer : public DebugWindow {
public:
    explicit TextureViewer(const std::string &path, PLTexture *texture) {
        texture_ = texture;
        texture_path = path;
    }

    ~TextureViewer() override {
        plDeleteTexture(texture_, true);
    }

    void ReloadTexture(PLTextureFilter filter_mode) {
        if(filter_mode == filter_mode_) {
            return;
        }

        plDeleteTexture(texture_, true);
        texture_ = Display_LoadTexture(texture_path.c_str(), filter_mode);
        filter_mode_ = filter_mode;
    }

    void Display() override {
        ImGui::SetNextWindowSize(ImVec2(texture_->w + 64, texture_->h + 128), ImGuiCond_Once);
        ImGui::Begin(std::string("Texture Viewer##" + std::to_string(id_)).c_str(), &status_,
                ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar);

        if(ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Open...")) {
                    show_file = true;
                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("View")) {
                ImGui::SliderInt("Scale", &scale_, 1, 8);
                if(ImGui::BeginMenu("Filter Mode")) {
                    if(ImGui::MenuItem("Linear", nullptr, (filter_mode_ == PL_TEXTURE_FILTER_LINEAR))) {
                        ReloadTexture(PL_TEXTURE_FILTER_LINEAR);
                    }
                    if(ImGui::MenuItem("Nearest", nullptr, (filter_mode_ == PL_TEXTURE_FILTER_NEAREST))) {
                        ReloadTexture(PL_TEXTURE_FILTER_NEAREST);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Image(reinterpret_cast<ImTextureID>(texture_->internal.id), ImVec2(
                texture_->w * scale_, texture_->h * scale_));
        ImGui::Separator();
        ImGui::Text("Path: %s", texture_path.c_str());
        ImGui::Text("%dx%d", texture_->w, texture_->h);
        ImGui::Text("Size: %ukB (%luB)", (unsigned int)plBytesToKilobytes(texture_->size), (long unsigned) texture_->size);

        ImGui::End();
    }

protected:
private:
    PLTexture *texture_{nullptr};
    std::string texture_path;

    PLTextureFilter filter_mode_{PL_TEXTURE_FILTER_LINEAR};

    int scale_{1};
};

/************************************************************/
/* Settings */

void UI_DisplaySettings() {
    if(!show_settings) {
        return;
    }

    ImGui::Begin("Settings", &show_settings);

    static int res[2] = {0, 0};
    static bool fs;
    if(res[0] == 0 && res[1] == 0) {
        System_GetWindowSize(&res[0], &res[1], &fs);
    }

    ImGui::InputInt2("Resolution", res);
    ImGui::Checkbox("Fullscreen", &fs);

    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 32);
    if(ImGui::Button("Apply")) {
        System_SetWindowSize(&res[0], &res[1], fs);
    }
    ImGui::SameLine();
    if(ImGui::Button("Cancel")) {
        show_settings = false;
    }

    ImGui::End();
}

/************************************************************/
/* New Game */

static GameModeSetup mode;

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
                 ImGuiWindowFlags_NoDecoration
    );

#if 0
    ImGui::ListBoxHeader("Maps");
    ImGui::ListBoxFooter();
#endif

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
        Game_StartNewGame(&mode);
    }
    ImGui::SameLine();
    if(ImGui::Button("Cancel")) {
        show_new_game = false;
    }

    ImGui::End();
}

/************************************************************/

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
    file_list.clear();
    //plScanDirectory(GetBasePath(), "map", AddFilePath, true);
    //plScanDirectory(GetBasePath(), "pog", AddFilePath, true);
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
        u_assert(0, "todo\n");
    }
    ImGui::End();
}

void UI_DisplayDebugMenu(void) {
    /* keep vars in sync with console vars, in case they
     * are changed during startup or whenever... */
    static bool show_fps;
    show_fps = cv_debug_fps->b_value;

    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("New Game...")) {
                memset(&mode, 0, sizeof(GameModeSetup));
                mode.num_players = 1;
                show_new_game = true;
            }
            if(ImGui::MenuItem("Load Game...")) {}
            ImGui::Separator();
            if(ImGui::MenuItem("Open...")) { show_file = true; }
            ImGui::Separator();
            if(ImGui::MenuItem("Settings...")) { show_settings = true; }
            ImGui::Separator();
            if(ImGui::MenuItem("Quit")) { show_quit = true; }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Debug")) {
            ImGui::MenuItem("Show Console", "`", &show_console);
            if(ImGui::MenuItem("Show FPS", nullptr, &show_fps)) {
                plSetConsoleVariable(cv_debug_fps, show_fps ? "true" : "false");
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

        ImGui::EndMainMenuBar();
    }

    UI_DisplayFileBox();
    UI_DisplayQuitBox();
    UI_DisplayNewGame();
    UI_DisplaySettings();
    UI_DisplayConsole();

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

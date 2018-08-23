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
#include "main.h"
#include "PreferencesDialog.h"
#include "ConsolePanel.h"
#include "ViewportPanel.h"

#include <wx/splash.h>
#include <wx/aboutdlg.h>

#include <PL/platform.h>
#include <PL/platform_filesystem.h>

#include <ParticleEditor/ParticleEditor.h>

class PerspectiveViewportPanel : public ViewportPanel {
public:
    explicit PerspectiveViewportPanel(wxWindow *parent) : ViewportPanel(parent) {
        SetContextId(PORK_ED_CTX_WORLD);
    }

    void Draw() override {
        if(!IsShown()) {
            return;
        }

        ViewportPanel::Draw();

        /* HACK because trying to set context during init
         * is failing and everything is burning. */
        static bool init = false;
        if(!init) {
            InitPorkEditor();
            init = true;
        }

        PreDrawPork(GetTimerInterval());
        DrawPork();
        PostDrawPork();
    }

protected:
private:
};

class TopViewportPanel : public ViewportPanel {
public:
    explicit TopViewportPanel(wxWindow *parent) : ViewportPanel(parent) {
        SetContextId(PORK_ED_CTX_WORLD);
    }

    void Draw() override {
        ViewportPanel::Draw();
    }

protected:
private:
};

class SideViewportPanel : public ViewportPanel {
public:
    explicit SideViewportPanel(wxWindow *parent) : ViewportPanel(parent) {
        SetContextId(PORK_ED_CTX_WORLD);
    }

    void Draw() override {
        ViewportPanel::Draw();
    }

protected:
private:
};

/////////////////////////////////////////////////////////////////////////////////////////

class EditorFrame : public wxFrame {
public:
    EditorFrame(const wxPoint &pos, const wxSize &size);
    ~EditorFrame() override;

protected:
private:
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnPreferences(wxCommandEvent &event);
    void OnFile(wxCommandEvent &event);
    void OnTool(wxCommandEvent &event);

    PerspectiveViewportPanel *viewport_;

    wxAuiManager *aui_manager_;

    wxDECLARE_EVENT_TABLE();

public:
    PerspectiveViewportPanel *GetViewport() { return viewport_; }
};

wxBEGIN_EVENT_TABLE(EditorFrame, wxFrame)
EVT_MENU(wxID_EXIT, EditorFrame::OnExit)
EVT_MENU(wxID_ABOUT, EditorFrame::OnAbout)
EVT_MENU(wxID_PREFERENCES, EditorFrame::OnPreferences)
EVT_MENU(wxID_OPEN, EditorFrame::OnFile)
EVT_MENU(wxID_SAVE, EditorFrame::OnFile)
EVT_MENU(wxID_SAVEAS, EditorFrame::OnFile)

EVT_MENU(ID_MAIN_TOOL_PARTICLE, EditorFrame::OnTool)
wxEND_EVENT_TABLE()

EditorFrame::EditorFrame(const wxPoint &pos, const wxSize &size) :
        wxFrame(nullptr, ID_FRAME_MAIN, PORK_EDITOR_TITLE, pos, size) {
    wxBitmap app_bitmap;
    if(app_bitmap.LoadFile("./rc/icon.png", wxBITMAP_TYPE_PNG)) {
        wxIcon app_icon;
        app_icon.CopyFromBitmap(app_icon);
        SetIcon(app_icon);
    }

    aui_manager_ = new wxAuiManager(this);

    auto *menu_file = new wxMenu;
    {
        menu_file->Append(wxID_NEW);
        menu_file->Append(wxID_OPEN);
        menu_file->AppendSeparator();
        menu_file->Append(wxID_SAVE);
        menu_file->Append(wxID_SAVEAS);
        menu_file->AppendSeparator();
        menu_file->Append(wxID_CLOSE);
        menu_file->AppendSeparator();
        menu_file->Append(wxID_EXIT);
    }

    auto *menu_edit = new wxMenu;
    {
        menu_edit->Append(wxID_UNDO);
        menu_edit->Append(wxID_REDO);
        menu_edit->AppendSeparator();
        menu_edit->AppendCheckItem(ID_MAIN_TRANSFORM, "&Transform");
        menu_edit->AppendCheckItem(ID_MAIN_ROTATE, "&Rotate");
        menu_edit->AppendCheckItem(ID_MAIN_SCALE, "&Scale");
        menu_edit->AppendSeparator();
        menu_edit->Append(wxID_PREFERENCES);

        menu_edit->Check(ID_MAIN_TRANSFORM, true);
    }

    auto *menu_view = new wxMenu;
    {
        //menu_view->Append(ID_RELOAD, "&Reload Resources...");
        //menu_view->AppendSeparator();

        auto *menu_view_panels = new wxMenu;
        {
            menu_view_panels->Append(ID_MAIN_CONSOLE, "&Console");
        }

        menu_view->AppendSubMenu(menu_view_panels, "&Panels");
    }

    auto *menu_tools = new wxMenu;
    {
        wxMenuItem *tools_model = new wxMenuItem(menu_tools, ID_MAIN_TOOL_MODEL, "&Model Tool...");
        menu_tools->Append(tools_model);

        wxMenuItem *tools_texture = new wxMenuItem(menu_tools, ID_MAIN_TOOL_TEXTURE, "&Texture Tool...");
        menu_tools->Append(tools_texture);

        wxMenuItem *tools_particle = new wxMenuItem(menu_tools, ID_MAIN_TOOL_PARTICLE, "&Particle Editor...");
        menu_tools->Append(tools_particle);
    }

    auto *menu_help = new wxMenu;
    {
        menu_help->Append(wxID_ABOUT);
    }

    auto *menu_window = new wxMenu;
    {
        //menu_window->AppendCheckItem(ID_CONSOLE, "&Console");
    }

    auto *menu_bar = new wxMenuBar;
    menu_bar->Append(menu_file, "&File");
    menu_bar->Append(menu_edit, "&Edit");
    menu_bar->Append(menu_view, "&View");
    menu_bar->Append(menu_tools, "&Tools");
    menu_bar->Append(menu_window, "&Window");
    menu_bar->Append(menu_help, "&Help");
    SetMenuBar(menu_bar);

    CreateStatusBar(3);

    {   /* toolbar */
        wxAuiPaneInfo toolbar_info;
        toolbar_info.Caption("Toolbar");
        toolbar_info.ToolbarPane();
        toolbar_info.Top();
        toolbar_info.Position(0);

        auto *toolbar_file = new wxAuiToolBar(this);
        //toolbar_file->AddTool(wxID_NEW, "New Map", );
        toolbar_file->Realize();
        aui_manager_->AddPane(toolbar_file, toolbar_info);
    }

    wxAuiPaneInfo viewport_info;
    viewport_info.Center();
    viewport_info.CenterPane();
    viewport_info.Movable(true);
    viewport_info.Floatable(true);
    viewport_info.Dockable(true);
    viewport_info.CloseButton(false);
    viewport_ = new PerspectiveViewportPanel(this);
    aui_manager_->AddPane(viewport_, viewport_info);

    g_console_panel = new ConsolePanel(this);
    wxAuiPaneInfo console_info;
    console_info.Caption("Console");
    console_info.Bottom();
    console_info.Movable(true);
    console_info.Floatable(true);
    console_info.CloseButton(false);
    console_info.MaximizeButton(true);
    aui_manager_->AddPane(g_console_panel, console_info);

    aui_manager_->Update();

    viewport_->Initialize();
    viewport_->StartDrawing();
}

EditorFrame::~EditorFrame() {
    aui_manager_->UnInit();
}

void EditorFrame::OnExit(wxCommandEvent &event) {
    Close(true);
}

void EditorFrame::OnAbout(wxCommandEvent &event) {
    wxAboutDialogInfo info;
    info.SetName(_(PORK_EDITOR_TITLE));
    info.SetVersion(_(ENGINE_MAJOR_VERSION + "." + ENGINE_MINOR_VERSION));
    info.SetWebSite("http://talonbrave.info/", _("TalonBrave.info"));
    info.SetDescription("Editor for OpenHoW");

    info.AddDeveloper("Mark \"hogsy\" Sowden <markelswo@gmail.com>");

    wxAboutBox(info);
}

void EditorFrame::OnPreferences(wxCommandEvent &event) {
    static PreferencesDialog *preferences = nullptr;
    if(preferences == nullptr) {
        preferences = new PreferencesDialog(this);
    }
    preferences->Show();
}

void EditorFrame::OnFile(wxCommandEvent &event) {
    char default_path[PL_SYSTEM_MAX_PATH];
    if(plIsEmptyString(GetModPath())) {
        snprintf(default_path, sizeof(default_path), "%s/maps", GetBasePath());
    } else {
        snprintf(default_path, sizeof(default_path), "%s%s/maps", GetBasePath(), GetModPath());
    }

    switch(event.GetId()) {
        default:break;

        case wxID_NEW: {
            /* todo...
             * display box allowing the user to enter the name and
             * some basic properties for their new map (such as the
             * sky, name, weather and some other fun stuff)
             */
        } break;

        case wxID_OPEN: {
            wxFileDialog *file = new wxFileDialog(
                    this,
                    "Open Map",
                    default_path,
                    "",
                    "Map definition (*.pmd)|*.pmd|"
                    "Particle system (*.pps)|*.pps|"
                    "Model (*.fac;*.no2;*.vtx)|*.fac;*.no2;*.vtx",
                    wxFD_OPEN|wxFD_FILE_MUST_EXIST
            );
            if(file->ShowModal() == wxID_OK) {
                plParseConsoleString("map " + file->GetPath() + " edit");
            }
        } break;

        case wxID_CLOSE: {
            /* todo: unload it */
        } break;

        case wxID_SAVE: {
            /* todo: save it */
        } break;
    }
}

void EditorFrame::OnTool(wxCommandEvent &event) {
    switch(event.GetId()) {
        default:break;

        case ID_MAIN_TOOL_PARTICLE: {
            ParticleEditorFrame *particle_editor = new ParticleEditorFrame(this);
            particle_editor->Show();
        } break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

class EditorApp : public wxApp {
public:
    bool OnInit() override;

    static unsigned int GetTicks() {
        if(main_frame_ == nullptr) {
            return 0;
        }

        return static_cast<unsigned int>(main_frame_->GetViewport()->GetTimerInterval());;
    }

    static void DisplayMessageBox(unsigned int level, const char *msg, ...) {
        switch(level) {
            case PORK_MBOX_ERROR: {
                level = wxICON_ERROR;
            } break;
            case PORK_MBOX_WARNING: {
                level = wxICON_WARNING;
            } break;
            case PORK_MBOX_INFORMATION: {
                level = wxICON_INFORMATION;
            } break;

            default: return;
        }

        char buf[2048];
        va_list args;
        va_start(args, msg);
        vsnprintf(buf, sizeof(buf), msg, args);
        va_end(args);

        wxMessageBox(buf, PORK_TITLE, level);
    }

    static void ShutdownLauncher() {
        /* do nothing ? */
    }

    static void SwapWindow() {
        main_frame_->GetViewport()->SwapBuffers();
    }

    const char *GetAppDataPath() { return &app_path_[0]; }

protected:
private:
    char app_path_[PL_SYSTEM_MAX_PATH];

    static EditorFrame *main_frame_;
};

EditorFrame *EditorApp::main_frame_ = nullptr;

bool EditorApp::OnInit() {
    plInitialize(argc, argv);

    EngineLauncherInterface interface{};
    memset(&interface, 0, sizeof(EngineLauncherInterface));
    interface.GetTicks          = EditorApp::GetTicks;
    interface.DisplayMessageBox = EditorApp::DisplayMessageBox;
    interface.ShutdownLauncher  = EditorApp::ShutdownLauncher;
    interface.SwapWindow        = EditorApp::SwapWindow;
    interface.mode              = PORK_MODE_LIMITED;

    plGetApplicationDataDirectory(PORK_APP_NAME, app_path_, PL_SYSTEM_MAX_PATH);

    if(!plCreatePath(app_path_)) {
        DisplayMessageBox(PORK_MBOX_WARNING, "Unable to create %s: %s\n"
                                             "Settings will not be saved.", app_path_, plGetError());
    }

    InitPork(argc, argv, interface);

    wxLog::SetLogLevel(wxLOG_Warning);

    wxInitAllImageHandlers();

    wxSplashScreen *splash = nullptr;
    wxBitmap splash_bitmap;
    if(splash_bitmap.LoadFile(pork_find("/splash.png"), wxBITMAP_TYPE_PNG)) {
        splash = new wxSplashScreen(
                splash_bitmap,
                wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_NO_TIMEOUT,
                0,
                nullptr,
                -1,
                wxDefaultPosition,
                wxDefaultSize,
                wxBORDER_SIMPLE | wxSTAY_ON_TOP
        );
    }

    main_frame_ = new EditorFrame(wxDefaultPosition, wxSize(1024, 768));
    main_frame_->Show(true);
    main_frame_->Maximize(true);
    SetTopWindow(main_frame_);

    delete splash;

    return true;
}

wxIMPLEMENT_APP(EditorApp);
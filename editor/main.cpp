/* OpenHOW
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

#include <wx/aui/aui.h>

#include <PL/platform.h>

class PerspectiveViewportPanel : public ViewportPanel {
public:
    PerspectiveViewportPanel(wxWindow *parent) : ViewportPanel(parent) {}

    virtual void Draw() {
        ViewportPanel::Draw();
    }

protected:
private:
};

class TopViewportPanel : public ViewportPanel {
public:
    TopViewportPanel(wxWindow *parent) : ViewportPanel(parent) {}

    virtual void Draw() {
        ViewportPanel::Draw();
    }

protected:
private:
};

class SideViewportPanel : public ViewportPanel {
public:
    SideViewportPanel(wxWindow *parent) : ViewportPanel(parent) {}

    virtual void Draw() {
        ViewportPanel::Draw();
    }

protected:
private:
};

/////////////////////////////////////////////////////////////////////////////////////////

class EditorFrame : public wxFrame {
public:
    EditorFrame(const wxPoint &pos, const wxSize &size);
    ~EditorFrame();

protected:
private:
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnPreferences(wxCommandEvent &event);

    wxAuiManager *aui_manager_;

    wxDECLARE_EVENT_TABLE();
};

enum {
    ID_MODEL,
    ID_TEXTURE,

    ID_WIREFRAME,
    ID_FLAT,
    ID_TEXTURED,
    ID_LIT,

    ID_RELOAD,

    ID_CONSOLE,

    ID_TRANSFORM,
    ID_ROTATE,
    ID_SCALE,
};

wxBEGIN_EVENT_TABLE(EditorFrame, wxFrame)
EVT_MENU(wxID_EXIT, EditorFrame::OnExit)
EVT_MENU(wxID_ABOUT, EditorFrame::OnAbout)
EVT_MENU(wxID_PREFERENCES, EditorFrame::OnPreferences)
wxEND_EVENT_TABLE()

EditorFrame::EditorFrame(const wxPoint &pos, const wxSize &size) :
        wxFrame(nullptr, wxID_ANY, PORK_EDITOR_TITLE, pos, size) {

    aui_manager_ = new wxAuiManager(this);

#if 0
    wxBitmap splash;
    if(splash.LoadFile("rc/splash.png", wxBITMAP_TYPE_PNG)) {
        new wxSplashScreen(
                splash,
                wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
                2000,
                NULL,
                wxID_ANY,
                wxDefaultPosition, wxDefaultSize,
                wxBORDER_SIMPLE | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR);
    }
#endif

    /////////////////////////////////////////////

    auto *toolbar = new wxToolBar();
    toolbar->Realize();
    SetToolBar(toolbar);

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
        menu_edit->AppendCheckItem(ID_TRANSFORM, "&Transform");
        menu_edit->AppendCheckItem(ID_ROTATE, "&Rotate");
        menu_edit->AppendCheckItem(ID_SCALE, "&Scale");
        menu_edit->AppendSeparator();
        menu_edit->Append(wxID_PREFERENCES);

        menu_edit->Check(ID_TRANSFORM, true);
    }

    auto *menu_view = new wxMenu;
    {
        menu_view->Append(ID_RELOAD, "&Reload Resources...");
        menu_view->AppendSeparator();

        auto *menu_view_panels = new wxMenu;
        {
            menu_view_panels->Append(ID_CONSOLE, "&Console");
        }

        menu_view->AppendSubMenu(menu_view_panels, "&Panels");
    }

    auto *menu_tools = new wxMenu;
    {
        wxMenuItem *tools_model = new wxMenuItem(menu_tools, ID_MODEL, "&Model Tool...");
        menu_tools->Append(tools_model);

        wxMenuItem *tools_texture = new wxMenuItem(menu_tools, ID_TEXTURE, "&Texture Tool...");
        menu_tools->Append(tools_texture);
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

    ////////////////////////////////////////////
    // Create the toolbar...

    wxAuiPaneInfo toolbar_info;
    toolbar_info.Caption("Toolbar");
    toolbar_info.ToolbarPane();
    toolbar_info.Top();

    auto *toolbar_file = new wxAuiToolBar(this);
    {
        toolbar_info.Position(0);
    }
    aui_manager_->AddPane(toolbar_file, toolbar_info);

    ////////////////////////////////////////////

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
}

EditorFrame::~EditorFrame() {
    aui_manager_->UnInit();
}

void EditorFrame::OnExit(wxCommandEvent &event) {
    Close(true);
}

void EditorFrame::OnAbout(wxCommandEvent &event) {
    wxMessageBox("blah", PORK_EDITOR_TITLE, wxOK | wxICON_INFORMATION);
}

void EditorFrame::OnPreferences(wxCommandEvent &event) {
    static PreferencesDialog *preferences = nullptr;
    if(preferences == nullptr) {
        preferences = new PreferencesDialog(this);
    }
    preferences->Show();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Pork Interface

unsigned int IGetTicks() {
    static wxTimer timer;
    if(!timer.IsRunning()) {
        timer.Start();
    }
    return static_cast<unsigned int>(timer.GetInterval());
}

void IDisplayMessageBox(unsigned int level, const char *msg, ...) {
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

void IShutdownLauncher() {

}

void ISwapWindow() {

}

/////////////////////////////////////////////////////////////////////////////////////////

class EditorApp : public wxApp {
public:
    bool OnInit() override;

protected:
private:
    EditorFrame *main_frame_{};
};

bool EditorApp::OnInit() {
    plInitialize(argc, argv);

    wxLog::SetLogLevel(wxLOG_Warning);

    wxInitAllImageHandlers();

    int width, height;
    wxDisplaySize(&width, &height);

    main_frame_ = new EditorFrame(wxDefaultPosition, wxSize(1024, 768));
    main_frame_->Show(true);
    main_frame_->Maximize(true);

    SetTopWindow(main_frame_);

    PorkLauncherInterface interface{};
    memset(&interface, 0, sizeof(PorkLauncherInterface));
    interface.GetTicks          = IGetTicks;
    interface.DisplayMessageBox = IDisplayMessageBox;
    interface.ShutdownLauncher  = IShutdownLauncher;
    interface.SwapWindow        = ISwapWindow;

    InitPork(argc, argv, interface);
    return true;
}

wxIMPLEMENT_APP(EditorApp);
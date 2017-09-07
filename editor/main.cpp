/*  Copyright (C) 2017 Mark E Sowden    */

#include "main.h"
#include "PreferencesDialog.h"
#include "ConsolePanel.h"

#include <wx/aui/aui.h>

class PIGEditorFrame : public wxFrame {
public:
    PIGEditorFrame(const wxPoint &pos, const wxSize &size);

protected:
private:
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnPreferences(wxCommandEvent &event);

    wxAuiManager *aui_manager_;

    wxDECLARE_EVENT_TABLE();
};

enum {
    EVENT_OPEN_MODELVIEWER,
    EVENT_OPEN_TEXTUREVIEWER,
};

wxBEGIN_EVENT_TABLE(PIGEditorFrame, wxFrame)
EVT_MENU(wxID_EXIT, PIGEditorFrame::OnExit)
EVT_MENU(wxID_ABOUT, PIGEditorFrame::OnAbout)
EVT_MENU(wxID_PREFERENCES, PIGEditorFrame::OnPreferences)
wxEND_EVENT_TABLE()

PIGEditorFrame::PIGEditorFrame(const wxPoint &pos, const wxSize &size) :
        wxFrame(nullptr, wxID_ANY, APP_TITLE, pos, size) {

    //aui_manager_ = new wxAuiManager(this);

    wxToolBar *toolbar = new wxToolBar();
    toolbar->Realize();
    SetToolBar(toolbar);

    wxMenu *menu_file = new wxMenu;
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

    wxMenu *menu_edit = new wxMenu;
    {
        menu_edit->Append(wxID_UNDO);
        menu_edit->Append(wxID_REDO);
        menu_edit->AppendSeparator();
        menu_edit->Append(wxID_PREFERENCES);
    }

    wxMenu *menu_view = new wxMenu;
    {}

    wxMenu *menu_help = new wxMenu;
    {
        menu_help->Append(wxID_ABOUT);
    }

    wxMenu *menu_tools = new wxMenu;
    {
        wxMenuItem *tools_model = new wxMenuItem(menu_tools, EVENT_OPEN_MODELVIEWER, "&Model Tool...");
        wxMenuItem *tools_texture = new wxMenuItem(menu_tools, EVENT_OPEN_TEXTUREVIEWER, "&Texture Tool...");

        menu_tools->Append(tools_model);
        menu_tools->Append(tools_texture);
    }

    wxMenu *menu_window = new wxMenu;
    {

    }

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(menu_file, "&File");
    menu_bar->Append(menu_edit, "&Edit");
    menu_bar->Append(menu_view, "&View");
    menu_bar->Append(menu_tools, "&Tools");
    menu_bar->Append(menu_help, "&Help");
    SetMenuBar(menu_bar);

    ConsolePanel *console = new ConsolePanel(this);
    wxAuiPaneInfo console_info;
    console_info.Caption("Console");
    console_info.Bottom();
    console_info.Movable(true);
    console_info.Floatable(true);
    console_info.CloseButton(false);
    console_info.MaximizeButton(true);

    //aui_manager_->AddPane(console, console_info);

    //aui_manager_->Update();

    CreateStatusBar();
}

void PIGEditorFrame::OnExit(wxCommandEvent &event) {
    Close(true);
}

void PIGEditorFrame::OnAbout(wxCommandEvent &event) {
    wxMessageBox("blah", APP_TITLE, wxOK | wxICON_INFORMATION);
}

void PIGEditorFrame::OnPreferences(wxCommandEvent &event) {
    static PreferencesDialog *preferences = nullptr;
    if(preferences == nullptr) {
        preferences = new PreferencesDialog(this);
    }
    preferences->Show();
}

/////////////////////////////////////////////////////////////////////////////////////////

class PIGEditorApp : public wxApp {
public:
    virtual bool OnInit();

protected:
private:
    PIGEditorFrame *main_frame_;
};

bool PIGEditorApp::OnInit() {
    int width, height;
    wxDisplaySize(&width, &height);
    main_frame_ = new PIGEditorFrame(wxDefaultPosition, wxSize(1024, 768));
    main_frame_->Show(true);
    main_frame_->Maximize(true);

    SetTopWindow(main_frame_);

    wxImage::AddHandler(new wxGIFHandler);
    wxImage::AddHandler(new wxICOHandler);
    wxImage::AddHandler(new wxTGAHandler);
    wxImage::AddHandler(new wxPNGHandler);

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_APP(PIGEditorApp);
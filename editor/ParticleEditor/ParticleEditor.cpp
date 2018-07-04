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

#include "ParticleEditor.h"

wxBEGIN_EVENT_TABLE(ParticleEditorFrame, wxFrame)
EVT_MENU(wxID_OPEN, ParticleEditorFrame::FileEvent)
EVT_MENU(wxID_SAVE, ParticleEditorFrame::FileEvent)
EVT_MENU(wxID_EXIT, ParticleEditorFrame::FileEvent)
EVT_MENU(wxID_CLOSE, ParticleEditorFrame::FileEvent)
wxEND_EVENT_TABLE()

ParticleEditorFrame::ParticleEditorFrame(wxWindow *parent) :
    wxFrame(parent, ID_FRAME_PARTICLE, "Particle Editor", wxDefaultPosition, wxSize(1024, 640))
{
    aui_manager = new wxAuiManager(this);

    wxMenu *file_menu = new wxMenu;
    file_menu->Append(wxID_NEW);
    file_menu->Append(wxID_OPEN);
    file_menu->AppendSeparator();
    file_menu->Append(wxID_CLOSE);
    file_menu->AppendSeparator();
    file_menu->Append(wxID_SAVE);
    file_menu->Append(wxID_SAVEAS);
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT);

    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, "&File");
    menu_bar->Append(file_menu, "&Edit");
    menu_bar->Append(file_menu, "&View");
    menu_bar->Append(file_menu, "&Help");

    SetMenuBar(menu_bar);

    Show();

    wxAuiPaneInfo viewport_info;
    viewport_info.CloseButton(false);
    viewport_info.PinButton(false);
    viewport_info.MaximizeButton(true);

    viewport_info.Caption("Viewport");

    viewport_info.Top();
    viewport_info.Right();

    viewport_ = new ParticleViewportPanel(this);
    aui_manager->AddPane(viewport_, viewport_info);

    CreateStatusBar(3);
    SetStatusText("Initialized", 0);

    Layout();

    aui_manager->Update();
}

ParticleEditorFrame::~ParticleEditorFrame() {
    aui_manager->UnInit();
}

bool ParticleEditorFrame::Destroy() {


    return wxTopLevelWindowBase::Destroy();
}

void ParticleEditorFrame::FileEvent(wxCommandEvent &event) {
    char default_path[PL_SYSTEM_MAX_PATH];
    if(plIsEmptyString(GetModPath())) {
        snprintf(default_path, sizeof(default_path), "%s/effects", GetBasePath());
    } else {
        snprintf(default_path, sizeof(default_path), "%s%s/effects", GetBasePath(), GetModPath());
    }

    switch(event.GetId()) {
        default:break;

        case wxID_OPEN: {
            wxFileDialog *file = new wxFileDialog(
                    this,
                    "Open Particle System",
                    default_path,
                    "",
                    "Supported files (*.pps)|*.pps",
                    wxFD_OPEN|wxFD_FILE_MUST_EXIST
            );
            if(file->ShowModal() == wxID_OK) {
                /* todo: load it in */

                wxString path = file->GetPath();

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
/* OpenHOW
 * Copyright (C) 2017 Mark E Sowden
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
#include "PreferencesDialog.h"

wxBEGIN_EVENT_TABLE(PreferencesDialog, wxDialog)
EVT_BUTTON(wxID_APPLY, PreferencesDialog::HandleCommandEvent)
EVT_BUTTON(wxID_OK, PreferencesDialog::HandleCommandEvent)
EVT_BUTTON(wxID_CANCEL, PreferencesDialog::HandleCommandEvent)
wxEND_EVENT_TABLE()

PreferencesDialog::PreferencesDialog(wxWindow *parent) :
        wxDialog(parent, wxID_ANY, "Preferences", wxDefaultPosition, wxSize(512, 512)) {

    SetSizeHints(wxSize(512, 512), wxSize(512, 512));

    notebook_ = new wxNotebook(this, wxID_ANY);

    ///////////////////////////////////////////////
    // autio settings

    panel_audio_ = new wxPanel(notebook_);

    ///////////////////////////////////////////////
    // input settings

    panel_input_ = new wxPanel(notebook_);

    ///////////////////////////////////////////////
    // general settings

    panel_general_ = new wxPanel(notebook_);

    auto_reload_ = new wxCheckBox(panel_general_, wxID_ANY, "Automatically reload documents");
    auto_reload_delay_ = new wxSpinCtrl(panel_general_, wxID_ANY, "test", wxDefaultPosition, wxSize(64, wxDefaultSize.GetHeight()));

    wxBoxSizer *auto_reload_delay_box = new wxBoxSizer(wxHORIZONTAL);
    auto_reload_delay_box->Add(auto_reload_delay_, 0, wxALL);
    auto_reload_delay_box->Add(new wxStaticText(panel_general_, wxID_ANY, "Delay"));

    wxStaticBoxSizer *auto_reload_box = new wxStaticBoxSizer(wxVERTICAL, panel_general_, "Auto Reload");
    auto_reload_box->Add(auto_reload_, 0, wxALL, 5);
    auto_reload_box->Add(auto_reload_delay_box);

    wxGridSizer *general_grid = new wxGridSizer(2, 2, 0, 0);
    general_grid->Add(auto_reload_box, 1, wxEXPAND | wxALL, 5);

    panel_general_->SetSizer(general_grid);
    panel_general_->Layout();

    ///////////////////////////////////////////////

    notebook_->AddPage(panel_general_, "General", true);
    notebook_->AddPage(panel_audio_, "Audio");

    wxBoxSizer *main_grid = new wxBoxSizer(wxVERTICAL);
    main_grid->Add(notebook_);

    wxButton *button_apply = new wxButton(this, wxID_APPLY);
    wxButton *button_okay = new wxButton(this, wxID_OK);
    wxButton *button_cancel = new wxButton(this, wxID_CANCEL);

    wxStdDialogButtonSizer *button_sizer = new wxStdDialogButtonSizer();
    button_sizer->AddButton(button_apply);
    button_sizer->AddButton(button_okay);
    button_sizer->AddButton(button_cancel);
    button_sizer->Realize();

    main_grid->Add(button_sizer, 0, wxBOTTOM | wxRIGHT | wxALIGN_RIGHT | wxEXPAND, 10);

    SetSizer(main_grid);

    Layout();
    Centre();
}

bool PreferencesDialog::Show() {
    SyncSettings();

    return wxDialog::Show();
}

void PreferencesDialog::HandleCommandEvent(wxCommandEvent &event) {
    switch(event.GetId()) {
        default: break;

        case wxID_OK: {
            ApplySettings();
            Hide();
            break;
        }

        case wxID_CANCEL: {
            Hide();
            break;
        }

        case wxID_APPLY: {
            ApplySettings();
            SyncSettings();
            break;
        }
    }
}

void PreferencesDialog::SyncSettings() {

}

void PreferencesDialog::ApplySettings() {

}

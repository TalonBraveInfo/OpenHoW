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
#pragma once

#include "main.h"

class PreferencesDialog : public wxDialog {
public:
    PreferencesDialog(wxWindow *parent);

    virtual bool Show();

    void SyncSettings();
    void ApplySettings();

private:
    // Events
    void HandleCommandEvent(wxCommandEvent &event);

    wxNotebook *notebook_;

    wxCheckBox *auto_reload_;
    wxSpinCtrl *auto_reload_delay_;

    // Video
    wxCheckBox *video_psx_mode;
    wxCheckBox *video_alpha_trick;

    // Panels
    wxPanel *panel_video;
    wxPanel *panel_audio_;
    wxPanel *panel_input_;
    wxPanel *panel_general_;

    wxDECLARE_EVENT_TABLE();
};

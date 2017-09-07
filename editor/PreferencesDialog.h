/*  Copyright (C) 2017 Mark E Sowden    */

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

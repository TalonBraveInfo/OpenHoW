/*  Copyright (C) 2017 Mark E Sowden    */

#pragma once

#include "main.h"

class ConsolePanel : public wxPanel {
public:
    ConsolePanel(wxWindow *parent);

    void PrintMessage(std::string msg);
    void PrintWarning(std::string war);
    void PrintError(std::string err);

private:
    void OnCommand(wxCommandEvent &event);
    void OnKey(wxKeyEvent &event);

    wxComboBox *c_in_;
    wxTextCtrl *c_out_;

    wxDECLARE_EVENT_TABLE();
};
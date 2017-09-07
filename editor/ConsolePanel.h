/*  Copyright (C) 2017 Mark E Sowden    */

#pragma once

#include "main.h"

class ConsolePanel : public wxPanel {
public:
    ConsolePanel(wxWindow *parent);

    void PrintMessage(std::string msg);
    void PrintWarning(std::string war);
    void PrintError(std::string err);

    wxTextCtrl *c_in_, *c_out_;

private:
    void OnCommand(wxCommandEvent &event);
    void OnKey(wxKeyEvent &event);

    wxDECLARE_EVENT_TABLE();
};
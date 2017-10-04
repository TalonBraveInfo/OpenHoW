/*  Copyright (C) 2017 Mark E Sowden    */

#include <PL/platform_console.h>

#include "ConsolePanel.h"

enum {
    CONSOLE_EVENT_INPUT,
    CONSOLE_EVENT_COMMAND
};

wxBEGIN_EVENT_TABLE(ConsolePanel, wxPanel)
    //EVT_CHAR_HOOK(ConsolePanel::OnKey)

    EVT_BUTTON(CONSOLE_EVENT_COMMAND, ConsolePanel::OnCommand)
    EVT_TEXT_ENTER(CONSOLE_EVENT_INPUT, ConsolePanel::OnCommand)
wxEND_EVENT_TABLE()

ConsolePanel::ConsolePanel(wxWindow *parent) : wxPanel(parent) {

    c_out_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_DONTWRAP | wxTE_AUTO_URL);
    c_out_->SetBackgroundColour(wxColour(0, 0, 0));
    c_out_->SetForegroundColour(wxColour(128, 128, 128));

    c_in_ = new wxComboBox(this, CONSOLE_EVENT_INPUT, "", wxDefaultPosition, wxDefaultSize, 0, NULL,
                           wxCB_SORT | wxTE_PROCESS_ENTER
    );

    size_t num_c;

    PLConsoleCommand **cmds;
    plGetConsoleCommands(&cmds, &num_c);
    for(PLConsoleCommand **var = cmds; var < cmds + num_c; ++var) {
        c_in_->AppendString((*var)->cmd);
    }

    PLConsoleVariable **vars;
    plGetConsoleVariables(&vars, &num_c);
    for(PLConsoleVariable **var = vars; var < vars + num_c; ++var) {
        c_in_->AppendString((*var)->var);
    }

    c_in_->AutoComplete(c_in_->GetStrings());

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(c_out_, 1, wxEXPAND | wxTOP | wxBOTTOM | wxLEFT | wxRIGHT);

    wxBoxSizer *h_sizer = new wxBoxSizer(wxHORIZONTAL);
    h_sizer->Add(new wxButton(this, CONSOLE_EVENT_COMMAND, "Submit"), 0, wxRIGHT);
    h_sizer->Add(c_in_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP);
    h_sizer->SetSizeHints(this);

    sizer->Add(h_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP);

    SetSizer(sizer);
    SetSize(wxSize(wxDefaultSize.x, 256));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsolePanel::OnCommand(wxCommandEvent &event) {
    if(event.GetEventType() == wxEVT_TEXT_ENTER) {
        // todo, pass command onto platform lib
        c_in_->SetValue("");

    }
}

void ConsolePanel::OnKey(wxKeyEvent &event) {
    event.DoAllowNextEvent();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsolePanel::PrintMessage(std::string msg) {
    c_out_->SetDefaultStyle(wxTextAttr(*wxWHITE));
    c_out_->AppendText(msg);
}

void ConsolePanel::PrintWarning(std::string war) {
    c_out_->SetDefaultStyle(wxTextAttr(*wxYELLOW));
    c_out_->AppendText(war);
}

void ConsolePanel::PrintError(std::string err) {
    c_out_->SetDefaultStyle(wxTextAttr(*wxRED));
    c_out_->AppendText(err);
}
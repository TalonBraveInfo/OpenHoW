/*  Copyright (C) 2017 Mark E Sowden    */

#include "ConsolePanel.h"

enum {
    CONSOLE_EVENT_INPUT,
    CONSOLE_EVENT_COMMAND
};

wxBEGIN_EVENT_TABLE(ConsolePanel, wxPanel)
    EVT_CHAR_HOOK(ConsolePanel::OnKey)

    EVT_BUTTON(CONSOLE_EVENT_COMMAND, ConsolePanel::OnCommand)
wxEND_EVENT_TABLE()

ConsolePanel::ConsolePanel(wxWindow *parent) : wxPanel(parent) {
    c_out_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_DONTWRAP | wxTE_AUTO_URL);
    c_out_->SetBackgroundColour(wxColour(0, 0, 0));
    c_out_->SetForegroundColour(wxColour(128, 128, 128));

    c_in_ = new wxTextCtrl(this, CONSOLE_EVENT_INPUT, "");

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(c_out_, 1, wxEXPAND | wxTOP | wxBOTTOM | wxLEFT | wxRIGHT);

    wxBoxSizer *h_sizer = new wxBoxSizer(wxHORIZONTAL);
    h_sizer->Add(new wxButton(this, CONSOLE_EVENT_COMMAND, "Submit"), 0, wxRIGHT);
    h_sizer->SetSizeHints(this);

    sizer->Add(h_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP);

    SetSizer(sizer);
    SetSize(wxSize(wxDefaultSize.x, 256));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsolePanel::OnCommand(wxCommandEvent &event) {
    // todo, pass command onto platform lib
    c_in_->Clear();
}

void ConsolePanel::OnKey(wxKeyEvent &event) {
    event.DoAllowNextEvent();
    switch(event.GetId()) {
        default: break;

        case CONSOLE_EVENT_INPUT: {
            if(event.GetKeyCode() == WXK_RETURN) {
                // todo, pass command onto platform lib
                c_in_->Clear();
            }
        }
    }
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
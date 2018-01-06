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
#include <PL/platform_console.h>

#include "ConsolePanel.h"

ConsolePanel *g_console_panel = nullptr;

void ConsoleCallbackFunction(int level, const char *msg) {
    switch(level) {
        default:
            g_console_panel->PrintMessage(msg);
            break;

        case PORK_LOG_ENGINE_ERROR:
            g_console_panel->PrintError(msg);
            break;

        case PORK_LOG_ENGINE_WARNING:
            g_console_panel->PrintWarning(msg);
            break;
    }
}

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

    c_in_ = new wxComboBox(this, CONSOLE_EVENT_INPUT, "", wxDefaultPosition, wxDefaultSize, 0, nullptr,
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

    auto *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(c_out_, 1, wxEXPAND | wxTOP | wxBOTTOM | wxLEFT | wxRIGHT);

    auto *h_sizer = new wxBoxSizer(wxHORIZONTAL);
    h_sizer->Add(new wxButton(this, CONSOLE_EVENT_COMMAND, "Submit"), 0, wxRIGHT);
    h_sizer->Add(c_in_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP);
    h_sizer->SetSizeHints(this);

    sizer->Add(h_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP);

    SetSizer(sizer);
    SetSize(wxSize(wxDefaultSize.x, 256));

    plSetConsoleOutputCallback(ConsoleCallbackFunction);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConsolePanel::OnCommand(wxCommandEvent &event) {
    if(event.GetEventType() == wxEVT_TEXT_ENTER) {
        plParseConsoleString(c_in_->GetValue());
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
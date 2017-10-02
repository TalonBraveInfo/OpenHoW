//
// Created by hogsy on 15/09/17.
//

#include "HexChoiceBox.h"

HexChoiceBox::HexChoiceBox(wxWindow *parent) : wxWindow(parent, wxID_ANY) {
    int x, y;
    wxGetMousePosition(&x, &y);
    SetPosition(wxPoint(x, y));
    SetSize(64, 64);
}


#include "HexChoiceBox.h"

HexChoiceBox::HexChoiceBox(wxWindow *parent) : wxControl() {

    SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);

    Create(parent, wxID_ANY); // must be called AFTER the above, not before!

    int x, y;
    wxGetMousePosition(&x, &y);
    SetPosition(wxPoint(x, y));
    SetSize(64, 64);

    Show(true);
}

HexChoiceBox::~HexChoiceBox() {

}

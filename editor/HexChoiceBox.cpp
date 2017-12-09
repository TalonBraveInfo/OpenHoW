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

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
#include "DrawCanvas.h"

wxGLContext *cur_context = nullptr;

wxBEGIN_EVENT_TABLE(DrawCanvas, wxGLCanvas) wxEND_EVENT_TABLE()

DrawCanvas::DrawCanvas(wxWindow *parent, int *attrib_list) :
        wxGLCanvas(parent, wxID_ANY, attrib_list) {

}

void DrawCanvas::SetCurrent() {
    cur_context->SetCurrent(*this);

    const wxSize size = GetClientSize();
    width_ = static_cast<unsigned int>(size.GetWidth());
    height_ = static_cast<unsigned int>(size.GetHeight());

    // todo, set this up manually with pl viewport crap?
}

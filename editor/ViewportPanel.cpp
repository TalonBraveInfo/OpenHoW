/* OpenHOW
 * Copyright (C) 2017 Mark E Sowden
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
#include "ViewportPanel.h"

wxBEGIN_EVENT_TABLE(ViewportPanel, wxPanel)
                EVT_TIMER(-1, ViewportPanel::OnTimer)
wxEND_EVENT_TABLE()

ViewportPanel::ViewportPanel(wxWindow *parent, const wxSize &size) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxFULL_REPAINT_ON_RESIZE) {
    timer_ = new wxTimer(this);
}

ViewportPanel::~ViewportPanel() {
    timer_->Stop();
    delete timer_;
}

void ViewportPanel::Initialize() {
    static int attributes[] = {
            WX_GL_DEPTH_SIZE, 24,
            WX_GL_STENCIL_SIZE, 8,
            WX_GL_MIN_RED, 8,
            WX_GL_MIN_GREEN, 8,
            WX_GL_MIN_BLUE, 8,
            WX_GL_MIN_ALPHA, 8,
            WX_GL_MIN_ACCUM_RED, 8,
            WX_GL_MIN_ACCUM_GREEN, 8,
            WX_GL_MIN_ACCUM_BLUE, 8,
            WX_GL_MIN_ACCUM_ALPHA, 8,
#if 0
            WX_GL_DOUBLEBUFFER, 1,
#endif
    };
    canvas_ = new DrawCanvas(this, attributes);

    wxBoxSizer *vert = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hori = new wxBoxSizer(wxHORIZONTAL);

    vert->Add(hori, 0, wxTOP | wxLEFT | wxRIGHT);
    vert->Add(canvas_, 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT);

    SetSizer(vert);
}

void ViewportPanel::StartDrawing() {
    timer_->Start(25);
}

void ViewportPanel::StopDrawing() {
    timer_->Stop();
}

void ViewportPanel::OnTimer(wxTimerEvent &event) {
    if(!IsShown()) {
        return;
    }

    canvas_->SetCurrent();
}



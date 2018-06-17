/* OpenHoW
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
#include "ViewportPanel.h"

wxGLContext *cur_context = nullptr;

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
    };
    canvas_ = new wxGLCanvas(this, wxID_ANY, attributes);

    if(cur_context == nullptr) {
        cur_context = new wxGLContext(canvas_);
        //cur_context->SetCurrent(*canvas_);
    }

    wxBoxSizer *vert = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hori = new wxBoxSizer(wxHORIZONTAL);

    vert->Add(hori, 0, wxTOP | wxLEFT | wxRIGHT);
    vert->Add(canvas_, 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT);

    SetSizer(vert);
}

void ViewportPanel::SetContext() {
    cur_context->SetCurrent(*canvas_);
}

void ViewportPanel::StartDrawing() {
    timer_->Start(24);
}

void ViewportPanel::StopDrawing() {
    timer_->Stop();
}

void ViewportPanel::OnTimer(wxTimerEvent &event) {
    if(!IsShown()) {
        return;
    }

    cur_context->SetCurrent(*canvas_);
#if 1
    const wxSize client_size = GetClientSize();
    UpdatePorkViewport(false,
                       static_cast<unsigned int>(client_size.GetWidth()),
                       static_cast<unsigned int>(client_size.GetHeight()));
#endif

    Draw();

    canvas_->SwapBuffers();
}

double ViewportPanel::GetTimerInterval() {
    return timer_->GetInterval();
}



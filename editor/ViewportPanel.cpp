/*  Copyright (C) 2017 Mark E Sowden    */

#include "ViewportPanel.h"

ViewportPanel::ViewportPanel(wxWindow *parent, const wxSize &size) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, size, wxFULL_REPAINT_ON_RESIZE) {
    timer_ = new wxTimer(this);
}

ViewportPanel::~ViewportPanel() {
    timer_->Stop();
    delete timer_;
}

void ViewportPanel::Initialize() {
    int attributes[] = {
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



/*  Copyright (C) 2017 Mark E Sowden    */

#include "DrawCanvas.h"

wxGLContext *cur_context = nullptr;

wxBEGIN_EVENT_TABLE(DrawCanvas, wxGLCanvas) wxEND_EVENT_TABLE()

DrawCanvas::DrawCanvas(wxWindow *parent, int *attrib_list) :
        wxGLCanvas(parent, wxID_ANY, attrib_list) {

}

void DrawCanvas::SetCurrent() {
    cur_context->SetCurrent(this);

    const wxSize size = GetClientSize();
    width_ = static_cast<unsigned int>(size.GetWidth());
    height_ = static_cast<unsigned int>(size.GetHeight());

    // todo, set this up manually with pl viewport crap?
}

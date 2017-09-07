/*  Copyright (C) 2017 Mark E Sowden    */

#pragma once

#include "main.h"

#include <wx/glcanvas.h>

class DrawCanvas : public wxGLCanvas {
public:
    DrawCanvas(wxWindow *parent, int *attrib_list);

    void SetCurrent();

protected:
private:
    unsigned int width_, height_;

    wxDECLARE_EVENT_TABLE();
};

/*  Copyright (C) 2017 Mark E Sowden    */

#pragma once

#include "main.h"
#include "DrawCanvas.h"

class ViewportPanel : public wxPanel {
public:
    ViewportPanel(wxWindow *parent, const wxSize &size = wxSize(512, 512));
    ~ViewportPanel();

    virtual void Initialize();
    virtual void Draw() {}

    void StartDrawing();
    void StopDrawing();

protected:
private:

    void OnTimer(wxTimerEvent &event);

    DrawCanvas *canvas_;

    wxTimer *timer_;

    wxDECLARE_EVENT_TABLE();
};

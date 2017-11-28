/* OpenHOW
 * Copyright (C) 2017-2018 Mark E Sowden
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

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
#pragma once

#include "main.h"

class ViewportPanel : public wxPanel {
public:
    ViewportPanel(wxWindow *parent, const wxSize &size = wxSize(512, 512));
    ~ViewportPanel();

    virtual void Initialize();
    virtual void Draw() {}

    void StartDrawing();
    void StopDrawing();

    double GetTimerInterval();

    void SwapBuffers();

    void SetContextId(PorkEdCtx context_id) {
        context_id_ = context_id;
    }

protected:

private:

    PorkEdCtx context_id_{PORK_ED_CTX_NONE};

    void OnTimer(wxTimerEvent &event);

    wxGLCanvas *canvas_{nullptr};
    wxTimer *timer_;

    wxDECLARE_EVENT_TABLE();
};

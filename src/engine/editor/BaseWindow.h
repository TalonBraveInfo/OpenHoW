// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#define ED_DEFAULT_WINDOW_FLAGS ImGuiWindowFlags_NoSavedSettings

struct ImVec2;

class BaseWindow {
public:
    explicit BaseWindow(BaseWindow *parent = nullptr, bool status = true) {
        static unsigned int windows_id_counter = 0;
        id_ = windows_id_counter++;
        status_ = status;
        parent_ = parent;
    }

    virtual ~BaseWindow() = default;

    virtual void Display() = 0;

    bool GetStatus() { return status_; }

    void SetWindowStatus( bool status) { status_ = status; }

    void Begin( const std::string &windowTitle, unsigned int flags );

    virtual void ToggleFullscreen();
    bool IsFullscreen() const { return isFullscreen; }

    void ToggleStatus() {
        status_ = !status_;
    }

protected:
    bool status_;
    unsigned int id_;

    BaseWindow *parent_{nullptr};

private:
	bool isFullscreen{ false };
	hei::Vector2 curWindowSize, curWindowPosition;
	hei::Vector2 oldSize, oldPosition;
};

#define dname(a)  std::string( a + "##" + /* std::to_string((ptrdiff_t)(this))*/ std::to_string(id_)).c_str()

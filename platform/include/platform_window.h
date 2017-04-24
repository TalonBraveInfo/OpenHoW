/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#include "platform.h"

#ifdef _WIN32
#else	// Linux

#	include <X11/Xlib.h>

#endif

typedef struct PLWindow {
    char *title, *classname;

    unsigned int width, height;

    int x, y;

    bool is_active, is_fullscreen;
    bool is_unlocked;

    int vsync_interval;

#ifdef _WIN32
    HWND	instance;
    HDC		dc;
#else	// Linux
    Window instance;
#endif
} PLWindow;

PL_EXTERN_C

PL_EXTERN PLvoid plMessageBox(const char *title, const char *msg, ...);

PL_EXTERN PLvoid plShowCursor(PLbool show);    // Shows or hides the cursor for the active window.
PL_EXTERN PLvoid plGetCursorPosition(int *x, int *y);

// Window Management
PL_EXTERN PLvoid plCreateWindow(PLWindow *window);

PL_EXTERN PLuint plGetScreenWidth(void);    // Returns width of current screen.
PL_EXTERN PLuint plGetScreenHeight(void);    // Returns height of current screen.
PL_EXTERN PLint plGetScreenCount(void);    // Returns number of avaliable screens.

// Rendering
PL_EXTERN PLvoid plSwapBuffers(PLWindow *window);

PL_EXTERN_C_END

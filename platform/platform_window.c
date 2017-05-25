/*	
Copyright (C) 2011-2016 OldTimes Software

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "PL/platform_window.h"
#include "PL/platform_log.h"
#include "PL/platform_math.h"

#if defined(__linux__)
#   include <X11/X.h>
#   include <X11/Xlib.h>
#endif

/*	Simple Window/Display Handling	*/
// todo, rewrite ALL of this.

#define PL_WINDOW_WIDTH    640
#define PL_WINDOW_HEIGHT   480

PLuint plGetScreenWidth(void) {
    plFunctionStart();
#ifdef _WIN32
    return GetSystemMetrics(SM_CXSCREEN);
#else
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        plSetError("Failed to open display!\n");
        return PL_WINDOW_WIDTH;
    }

    Screen *screen = DefaultScreenOfDisplay(display);
    if (!screen) {
        plSetError("Failed to get screen of display!\n");
        return PL_WINDOW_WIDTH;
    }

    return (PLuint) screen->width;
#endif
}

PLuint plGetScreenHeight(void) {
    plFunctionStart();
#ifdef _WIN32
    return GetSystemMetrics(SM_CYSCREEN);
#else
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        plSetError("Failed to open display!\n");
        return PL_WINDOW_HEIGHT;
    }

    Screen *screen = DefaultScreenOfDisplay(display);
    if (!screen) {
        plSetError("Failed to get screen of display!\n");
        return PL_WINDOW_HEIGHT;
    }

    return (PLuint) screen->height;
#endif
}

#if defined(__linux__) || defined(__APPLE__)
Display *dMainDisplay;
Window wRootWindow;
#endif

PLint plGetScreenCount(void) {
#ifdef _WIN32
    return GetSystemMetrics(SM_CMONITORS);
#else
    return XScreenCount(dMainDisplay);
#endif
}

/*
	Window Creation
*/

PL_INSTANCE iGlobalInstance;

int iActive = 0,    // Number of current active windows.
        iScreen;            // Default screen.

/*	Create a new window.
*/
void plCreateWindow(PLWindow *window) {
    plFunctionStart();

    // Make sure the window has been initialized.
    if (!window) {
        plSetError("Window has not been allocated!\n");
        return;
    }
#if 0
    // Make sure that any platform specific window systems are set up.
    else if()
    {
#ifdef __linux__
        dMainDisplay = XOpenDisplay(NULL);
        if(!dMainDisplay)
        {
            plSetError("Failed to open display!\n");
            return;
        }

        iScreen = DefaultScreen(dMainDisplay);
#else
#endif
    }
#endif

#ifdef _WIN32
    {
        WNDCLASSEX wWindowClass;

        wWindowClass.cbClsExtra		= 0;
        wWindowClass.cbSize			= sizeof(WNDCLASSEX);
        wWindowClass.cbWndExtra		= 0;
        wWindowClass.hbrBackground	= NULL;
        wWindowClass.hCursor		= LoadCursor(iGlobalInstance,IDC_ARROW);
        wWindowClass.hIcon			= LoadIcon(iGlobalInstance,IDI_APPLICATION);
        wWindowClass.hIconSm		= 0;
        wWindowClass.hInstance		= iGlobalInstance;
        wWindowClass.lpfnWndProc	= NULL;	// (WNDPROC)Platform_WindowProcedure;
        wWindowClass.lpszClassName	= window->classname;
        wWindowClass.lpszMenuName	= 0;
        wWindowClass.style			= CS_OWNDC;

        if(!RegisterClassEx(&wWindowClass))
        {
            plSetError("Failed to register window class!\n");
            return;
        }

        window->instance = CreateWindowEx(
            0,
            window->classname,
            window->title,
            WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
            window->x,
            window->y,
            window->width,
            window->height,
            NULL,NULL,
            iGlobalInstance,
            NULL);
        if (!window->instance)
        {
            plSetError("Failed to create window!\n");
            return;
        }

        UpdateWindow(window->instance);
        SetForegroundWindow(window->instance);

        window->dc = GetDC(window->instance);
    }
#else	// Linux
    {
        // Create our window.
        window->instance = XCreateSimpleWindow(
                dMainDisplay,
                RootWindow(dMainDisplay, iScreen),
                window->x,
                window->y,
                window->width,
                window->height,
                1,
                BlackPixel(dMainDisplay, iScreen),
                WhitePixel(dMainDisplay, iScreen));
        if (!window->instance) {
            plSetError("Failed to create window!\n");
            return;
        }

        // Set the window title.
        XStoreName(dMainDisplay, window->instance, window->title);
    }
#endif

    window->is_active = true;
}

/*  
*/
PLvoid plShowCursor(PLbool show) {
    static bool _cursorvisible = true;
    if (show == _cursorvisible)
        return;
#ifdef _WIN32
    ShowCursor(show);
#else	// Linux
#endif
    _cursorvisible = show;
}

/*	Gets the position of the cursor.
	TODO:
		Move into platform_input.
*/
void plGetCursorPosition(int *x, int *y) {
#ifdef _WIN32
    POINT	pPoint;
    GetCursorPos(&pPoint);
    *x = pPoint.x; *y = pPoint.y;
#else	// Linux
    // todo, implement me!
#endif
}

/*	Displays a simple dialogue window.
*/
void plMessageBox(const char *ccTitle, const char *ccMessage, ...) {
    char cOut[2048];
    va_list vlArguments;

    plFunctionStart();

    va_start(vlArguments, ccMessage);
    vsprintf(cOut, ccMessage, vlArguments);
    va_end(vlArguments);

    // Also print a message out, on the off chance the message box fails.
    printf("Platform: %s", cOut);

#ifndef _WIN32
    {
        int iDefaultScreen;
        XEvent xEvent;
        Display *display;
        Window wMessageWindow;

        display = XOpenDisplay(NULL);
        if (!display) {
            plSetError("Failed to open display!\n");
            return;
        }

        iDefaultScreen = DefaultScreen(display);

        wMessageWindow = XCreateSimpleWindow(
                display,
                RootWindow(display, iDefaultScreen),
                50, 50,
                512, 64,
                1,
                BlackPixel(display, iDefaultScreen),
                WhitePixel(display, iDefaultScreen));
        XStoreName(display, wMessageWindow, ccTitle);
        XSelectInput(display, wMessageWindow, ExposureMask | KeyPressMask);
        XMapWindow(display, wMessageWindow);

        for (;;) {
            XNextEvent(display, &xEvent);

            if (xEvent.type == Expose) {
                XDrawString(display, wMessageWindow, DefaultGC(display, iDefaultScreen), 10, 10, cOut,
                            (PLint) strlen(cOut));
                XDrawString(display, wMessageWindow, DefaultGC(display, iDefaultScreen), 10, 54,
                            "Press any key to continue...", 32);
            } else if (xEvent.type == KeyPress)
                break;
        }

        XCloseDisplay(display);
    }
#else   // Windows
    MessageBoxEx(NULL, cOut, ccTitle, MB_SETFOREGROUND | MB_ICONERROR, 0);
#endif
}

void plSwapBuffers(PLWindow *window) {
#ifdef _WIN32
    SwapBuffers(window->dc);
#else	// Linux
    //glXSwapBuffers() // todo
#endif
}

////////////////////////////////////////////////////////////////////

#include "PL/platform_log.h"
#define PL_WINDOW_LOG  "pl_window"
#ifdef _DEBUG
#	define plWindowLog(...) plWriteLog(PL_WINDOW_LOG, __VA_ARGS__)
#else
#   define plWindowLog(...)
#endif

#if !defined(_WIN32)

PLint _plHandleX11Error(Display *display, XErrorEvent *error) {
//    XGetErrorText()
    return 0;
}

#endif

PLresult _plInitWindow(void) {
    plFunctionStart();

    plClearLog(PL_WINDOW_LOG);

    XSetErrorHandler(_plHandleX11Error);

    return PL_RESULT_SUCCESS;
}

void _plShutdownWindow(void) {
    plFunctionStart();
}
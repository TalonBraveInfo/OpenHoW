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

#if defined(PL_USE_SDL2)
#   include <SDL2/SDL.h>
#else
#   if defined(__linux__)
#       include <X11/X.h>
#       include <X11/Xlib.h>
#   endif
#endif

/*	Simple Window/Display Handling	*/

#define PL_WINDOW_WIDTH    640
#define PL_WINDOW_HEIGHT   480

#if !defined(PL_USE_SDL2) && (defined(__linux__) || defined(__APPLE__))
Display *_plwindow_x11_display;
Window _plwindow_x11_root;
#endif

#include "PL/platform_log.h"
#define PL_WINDOW_LOG  "pl_window"
#ifdef _DEBUG
#	define plWindowLog(...) plWriteLog(PL_WINDOW_LOG, __VA_ARGS__)
#else
#   define plWindowLog(...)
#endif

PLresult _plInitWindow(void) {
    plClearLog(PL_WINDOW_LOG);

#if defined(PL_USE_SDL2)


    SDL_DisableScreenSaver();

#elif defined(__linux__) || defined(__APPLE__)
    // todo, finish...

    _plwindow_x11_display = XOpenDisplay(NULL);
    if(!_plwindow_x11_display) {
        _plSetErrorMessage("Failed to create display!\n");
        return PL_RESULT_DISPLAY;
    }

#endif

    return PL_RESULT_SUCCESS;
}

void _plShutdownWindow(void) {
#if defined(PL_USE_SDL2)
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
#endif
}

///////////////////////////////////////////////////////////

#define DEFAULT_SCREEN_WIDTH    640
#define DEFAULT_SCREEN_HEIGHT   480

unsigned int plGetScreenWidth(void) {
    plFunctionStart();
#if defined(PL_USE_SDL2)
    SDL_Rect screen;
    if(SDL_GetDisplayBounds(0, &screen) == 0) {
        plWindowLog("Failed to get display bounds!\n");
        return DEFAULT_SCREEN_WIDTH;
    }

    return (unsigned int) screen.w;
#else
#ifdef _WIN32
    return GetSystemMetrics(SM_CXSCREEN);
#else
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        _plSetErrorMessage("Failed to open display!\n");
        return PL_WINDOW_WIDTH;
    }

    Screen *screen = DefaultScreenOfDisplay(display);
    if (!screen) {
        _plSetErrorMessage("Failed to get screen of display!\n");
        return PL_WINDOW_WIDTH;
    }

    return (unsigned int) screen->width;
#endif
#endif
}

unsigned int plGetScreenHeight(void) {
    plFunctionStart();
#if defined(PL_USE_SDL2)
    SDL_Rect screen;
    if(SDL_GetDisplayBounds(0, &screen) == 0) {
        plWindowLog("Failed to get display bounds!\n");
        return DEFAULT_SCREEN_HEIGHT;
    }

    return (unsigned int) screen.h;
#else
#ifdef _WIN32
    return GetSystemMetrics(SM_CYSCREEN);
#else
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        _plSetErrorMessage("Failed to open display!\n");
        return PL_WINDOW_HEIGHT;
    }

    Screen *screen = DefaultScreenOfDisplay(display);
    if (!screen) {
        _plSetErrorMessage("Failed to get screen of display!\n");
        return PL_WINDOW_HEIGHT;
    }

    return (unsigned int) screen->height;
#endif
#endif
}

///////////////////////////////////////////////////////////

unsigned int plGetScreenCount(void) {
#if defined(PL_USE_SDL2)
    int screens = SDL_GetNumVideoDisplays();
    if(!screens) {
        return 0;
    }
    return (unsigned int) screens;
#else
#ifdef _WIN32
    return GetSystemMetrics(SM_CMONITORS);
#else
    return (unsigned int)XScreenCount(_plwindow_x11_display);
#endif
#endif
}

///////////////////////////////////////////////////////////

/*	Window Creation */

PLWindow *plCreateWindow(const char *title, int x, int y, unsigned int w, unsigned int h) {
    plFunctionStart();

    // todo, add to global pool
    PLWindow *window = (PLWindow*)malloc(sizeof(PLWindow));
    if (!window) { // Make sure the window has been initialized.
        _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate window! (%d)\n", sizeof(PLWindow));
        return NULL;
    }

    memset(window, 0, sizeof(PLWindow));

#if defined(PL_USE_SDL2)
    SDL_Window *sdl_window = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_OPENGL);
    if(!sdl_window) {
        plDeleteWindow(window);
        return NULL;
    }
    SDL_GL_MakeCurrent(sdl_window, SDL_GL_CreateContext(sdl_window));

    window->sys_id = SDL_GetWindowID(sdl_window);
#endif

    window->is_active = true;

    return window;
}

void plDeleteWindow(PLWindow *window) {
    if(!window) {
        return;
    }

#if defined(PL_USE_SDL2)
    SDL_Window *sdl_window = SDL_GetWindowFromID(window->sys_id);
    if(sdl_window) {
        SDL_DestroyWindow(sdl_window);
    }
#endif

    // todo, remove from global pool
    free(window);
}

///////////////////////////////////////////////////////////

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
            _plSetErrorMessage("Failed to open display!\n");
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
                            (int) strlen(cOut));
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
#if defined(PL_USE_SDL2)
    SDL_GL_SwapWindow(SDL_GetWindowFromID(window->sys_id));
#else
#ifdef _WIN32
    SwapBuffers(window->dc);
#else	// Linux
    //glXSwapBuffers() // todo
#endif
#endif
}
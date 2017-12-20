/* OpenHOW
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
#include <pork/pork.h>

#include <SDL2/SDL.h>

SDL_Window *window = NULL;
SDL_GLContext *context = NULL;

#define print(...)          plLogMessage(PORK_LOG_LAUNCHER, __VA_ARGS__)
#define print_warning(...)  plLogMessage(PORK_LOG_LAUNCHER_WARNING, __VA_ARGS__)
#define print_error(...)    plLogMessage(PORK_LOG_LAUNCHER_ERROR, __VA_ARGS__)

void IDisplayMessageBox(unsigned int level, const char *msg, ...) {
    switch(level) {
        case PORK_MBOX_ERROR: {
            level = SDL_MESSAGEBOX_ERROR;
        } break;
        case PORK_MBOX_WARNING: {
            level = SDL_MESSAGEBOX_WARNING;
        } break;
        case PORK_MBOX_INFORMATION: {
            level = SDL_MESSAGEBOX_INFORMATION;
        } break;

        default: return;
    }

    char buf[2048];
    va_list args;
    va_start(args, msg);
    vsnprintf(buf, sizeof(buf), msg, args);
    va_end(args);

    SDL_ShowSimpleMessageBox(level, PORK_TITLE, msg, window);
}

void IDisplayWindow(bool fullscreen, unsigned int width, unsigned int height) {
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    if(SDL_GL_SetSwapInterval(-1) != 0) {
        SDL_GL_SetSwapInterval(1);
    }

    unsigned int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS;
    if(fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    } else {
        flags |= SDL_WINDOW_SHOWN;
    }

    window = SDL_CreateWindow(
            PORK_TITLE,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            flags
    );
    if(window == NULL) {
        IDisplayMessageBox(PORK_MBOX_ERROR, "Failed to create window!\n%s", SDL_GetError());
        ShutdownPork();
    }

    if((context = SDL_GL_CreateContext(window)) == NULL) {
        IDisplayMessageBox(PORK_MBOX_ERROR, "Failed to create context!\n%s", SDL_GetError());
        ShutdownPork();
    }
}

void ISwapDisplay(void) {
    SDL_GL_SwapWindow(window);
}

void IShutdownLauncher(void) {
    if(context != NULL) {
        SDL_GL_DeleteContext(context);
    }

    if(window != NULL) {
        SDL_DestroyWindow(window);
    }

    SDL_ShowCursor(1);

    SDL_EnableScreenSaver();
    SDL_Quit();

    exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////

void PollEvents(void) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            default:break;

            case SDL_KEYUP:
            case SDL_KEYDOWN: {
                if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE && event.key.state == SDL_PRESSED) {
                    ShutdownPork();
                }


            } break;

            case SDL_MOUSEBUTTONDOWN: {

            } break;

            case SDL_MOUSEWHEEL: {

            } break;

            case SDL_MOUSEMOTION: {

            } break;

            case SDL_QUIT: {
                ShutdownPork();
            } break;

            case SDL_WINDOWEVENT: {
                if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    unsigned int flags = SDL_GetWindowFlags(window);
                    UpdatePorkViewport((bool) (flags & SDL_WINDOW_FULLSCREEN),
                                       (unsigned int) event.window.data1,
                                       (unsigned int) event.window.data2);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    plSetupLogOutput(PORK_LOG);

    plSetupLogLevel(PORK_LOG_LAUNCHER, "launcher", PLColour(0, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_LAUNCHER_WARNING, "launcher-warning", PLColour(255, 255, 0, 255), true);
    plSetupLogLevel(PORK_LOG_LAUNCHER_ERROR, "launcher-error", PLColour(255, 0, 0, 255), true);

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        IDisplayMessageBox(PORK_MBOX_ERROR, "Failed to initialize SDL2!\n%s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_DisableScreenSaver();

    PorkLauncherInterface interface;
    memset(&interface, 0, sizeof(PorkLauncherInterface));
    interface.DisplayMessageBox = IDisplayMessageBox;
    interface.DisplayWindow = IDisplayWindow;
    interface.SwapWindow = ISwapDisplay;
    interface.ShutdownLauncher = IShutdownLauncher;

    InitPork(argc, argv, interface);

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(0);

#define TICKS_PER_SECOND    25
#define SKIP_TICKS          (1000 / TICKS_PER_SECOND)
#define MAX_FRAMESKIP       5

    unsigned int next_tick = SDL_GetTicks();
    unsigned int loops;
    double delta_time;

    while(true) {
        PollEvents();

        // simulate the game at TICKS_PER_SECOND, might need adjusting
        loops = 0;
        while(SDL_GetTicks() > next_tick && loops < MAX_FRAMESKIP) {
            SimulatePork();
            next_tick += SKIP_TICKS;
            loops++;
        }

        delta_time = (double)(SDL_GetTicks() + SKIP_TICKS - next_tick) / (double)(SKIP_TICKS);
        DrawPork(delta_time);
    }

    ShutdownPork();

    return EXIT_SUCCESS;
}
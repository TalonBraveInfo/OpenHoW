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
#include <SDL2/SDL.h>
#include <PL/platform_filesystem.h>

#include "pork/pork.h"

SDL_Window *window = NULL;
SDL_GLContext *context = NULL;

/* controller support */
SDL_GameController *controller = NULL;

#define LogInfo(...)  plLogMessage(PORK_LOG_LAUNCHER, __VA_ARGS__)
#define LogWarn(...)  plLogMessage(PORK_LOG_LAUNCHER_WARNING, __VA_ARGS__)
#define Error(...)    plLogMessage(PORK_LOG_LAUNCHER_ERROR, __VA_ARGS__)

unsigned int IGetTicks(void) {
    return SDL_GetTicks();
}

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

#if 1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

#if 1
    if(SDL_GL_SetSwapInterval(-1) != 0) {
        SDL_GL_SetSwapInterval(1);
    }
#endif

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

void ISetWindowTitle(const char *title) {
    SDL_SetWindowTitle(window, title);
}

void ISwapDisplay(void) {
    SDL_GL_SwapWindow(window);
}

void IShutdownLauncher(void) {
    ShutdownPork();

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

int TranslateSDLKey(int key) {
    if(key < 128) {
        return key;
    }

    switch(key) {
        case SDLK_F1: return PORK_KEY_F1;
        case SDLK_F2: return PORK_KEY_F2;
        case SDLK_F3: return PORK_KEY_F3;
        case SDLK_F4: return PORK_KEY_F4;
        case SDLK_F5: return PORK_KEY_F5;
        case SDLK_F6: return PORK_KEY_F6;
        case SDLK_F7: return PORK_KEY_F7;
        case SDLK_F8: return PORK_KEY_F8;
        case SDLK_F9: return PORK_KEY_F9;
        case SDLK_F10: return PORK_KEY_F10;
        case SDLK_F11: return PORK_KEY_F11;
        case SDLK_F12: return PORK_KEY_F12;

        case SDLK_ESCAPE: return PORK_KEY_ESCAPE;

        case SDLK_PAUSE: return PORK_KEY_PAUSE;
        case SDLK_INSERT: return PORK_KEY_INSERT;
        case SDLK_HOME: return PORK_KEY_HOME;

        case SDLK_UP: return PORK_KEY_UP;
        case SDLK_DOWN: return PORK_KEY_DOWN;
        case SDLK_LEFT: return PORK_KEY_LEFT;
        case SDLK_RIGHT: return PORK_KEY_RIGHT;

        case SDLK_SPACE: return PORK_KEY_SPACE;

        case SDLK_LSHIFT: return PORK_KEY_LSHIFT;
        case SDLK_RSHIFT: return PORK_KEY_RSHIFT;

        case SDLK_PAGEUP: return PORK_KEY_PAGEUP;
        case SDLK_PAGEDOWN: return PORK_KEY_PAGEDOWN;

        default: return -1;
    }
}

int TranslateSDLButton(int button) {
    switch(button) {
        case SDL_CONTROLLER_BUTTON_A: return PORK_BUTTON_CROSS;
        case SDL_CONTROLLER_BUTTON_B: return PORK_BUTTON_CIRCLE;
        case SDL_CONTROLLER_BUTTON_X: return PORK_BUTTON_SQUARE;
        case SDL_CONTROLLER_BUTTON_Y: return PORK_BUTTON_TRIANGLE;

        case SDL_CONTROLLER_BUTTON_BACK: return PORK_BUTTON_SELECT;
        case SDL_CONTROLLER_BUTTON_START: return PORK_BUTTON_START;

        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return PORK_BUTTON_L1;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK: return PORK_BUTTON_L3;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return PORK_BUTTON_R1;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return PORK_BUTTON_R3;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return PORK_BUTTON_DOWN;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return PORK_BUTTON_LEFT;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return PORK_BUTTON_RIGHT;
        case SDL_CONTROLLER_BUTTON_DPAD_UP: return PORK_BUTTON_UP;

        default: return -1;
    }
}

void PollEvents(void) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            default:break;

            case SDL_KEYUP: {
                int key = TranslateSDLKey(event.key.keysym.sym);
                if(key != -1) {
                    SetPorkKeyState(key, false);
                }
            } break;

            case SDL_KEYDOWN: {
                int key = TranslateSDLKey(event.key.keysym.sym);
                if(key != -1) {
                    SetPorkKeyState(key, true);
                }
            } break;

            case SDL_TEXTINPUT: {

            } break;

            case SDL_MOUSEBUTTONDOWN: {

            } break;

            case SDL_MOUSEWHEEL: {

            } break;

            case SDL_MOUSEMOTION: {

            } break;

            case SDL_CONTROLLERBUTTONUP: {
                int button = TranslateSDLButton(event.cbutton.button);
                if(button != -1) {
                    SetPorkButtonState((unsigned int) event.cbutton.which, button, false);
                }
            } break;

            case SDL_CONTROLLERBUTTONDOWN: {
                int button = TranslateSDLButton(event.cbutton.button);
                if(button != -1) {
                    SetPorkButtonState((unsigned int) event.cbutton.which, button, true);
                }
            } break;

            case SDL_CONTROLLERAXISMOTION: {
                if(event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.value > 1000) {
                    SetPorkButtonState((unsigned int) event.cbutton.which, PORK_BUTTON_L2, true);
                } else if(event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.value <= 1000) {
                    SetPorkButtonState((unsigned int) event.cbutton.which, PORK_BUTTON_L2, false);
                }

                if(event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && event.caxis.value > 1000) {
                    SetPorkButtonState((unsigned int) event.cbutton.which, PORK_BUTTON_R2, true);
                } else if(event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && event.caxis.value <= 1000){
                    SetPorkButtonState((unsigned int) event.cbutton.which, PORK_BUTTON_R2, false);
                }
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
    char app_dir[PL_SYSTEM_MAX_PATH];
    plGetApplicationDataDirectory(PORK_APP_NAME, app_dir, PL_SYSTEM_MAX_PATH);

    if(!plCreatePath(app_dir)) {
        IDisplayMessageBox(PORK_MBOX_WARNING, "Unable to create %s: %s\n"
                                              "Settings will not be saved.", app_dir, plGetError());
    }

    char log_path[PL_SYSTEM_MAX_PATH];
    snprintf(log_path, PL_SYSTEM_MAX_PATH, "%s/" PORK_LOG, app_dir);
    plSetupLogOutput(log_path);

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
    interface.GetTicks          = IGetTicks;
    interface.DisplayMessageBox = IDisplayMessageBox;
    interface.DisplayWindow     = IDisplayWindow;
    interface.SwapWindow        = ISwapDisplay;
    interface.ShutdownLauncher  = IShutdownLauncher;
    interface.SetWindowTitle    = ISetWindowTitle;

    int num_joysticks = SDL_NumJoysticks();
    if(num_joysticks < 0) {
        LogWarn("%s\n", SDL_GetError());
    } else {
        if(SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") == -1) {
            LogWarn("%s\n", SDL_GetError());
        } else {
            for (int i = 0; i < num_joysticks; ++i) {
                if (SDL_IsGameController(i)) {
                    controller = SDL_GameControllerOpen(i);
                    if (controller == NULL) {
                        LogWarn("%s\n", SDL_GetError());
                    } else {
                        const char *name = SDL_GameControllerName(controller);
                        if (name == NULL) {
                            name = "unknown";
                        }

                        LogInfo("found controller: %s\n", name);
                        SDL_GameControllerEventState(SDL_ENABLE);
                        break;
                    }
                }
            }
        }
    }

    InitPork(argc, argv, interface);

    //SDL_SetRelativeMouseMode(SDL_TRUE);
    //SDL_ShowCursor(0);

    /* using this to catch modified keys
     * without having to do the conversion
     * ourselves                            */
    SDL_StartTextInput();

#define TICKS_PER_SECOND    25
#define SKIP_TICKS          (1000 / TICKS_PER_SECOND)
#define MAX_FRAMESKIP       5

    unsigned int next_tick = SDL_GetTicks();
    unsigned int loops;
    double delta_time;

    while(IsPorkRunning()) {
        PollEvents();

        // simulate the game at TICKS_PER_SECOND, might need adjusting
        loops = 0;
        while(SDL_GetTicks() > next_tick && loops < MAX_FRAMESKIP) {
            SimulatePork();
            next_tick += SKIP_TICKS;
            loops++;
        }

        delta_time = (double)(SDL_GetTicks() + SKIP_TICKS - next_tick) / (double)(SKIP_TICKS);
        PreDrawPork(delta_time);
        DrawPork();
        PostDrawPork();
    }

    SDL_StopTextInput();

    ShutdownPork();

    if(controller != NULL) {
        SDL_GameControllerClose(controller);
    }

    return EXIT_SUCCESS;
}
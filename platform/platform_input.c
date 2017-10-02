/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include <PL/platform_input.h>

#include <SDL2/SDL.h>

/* Input Wrapper */

#define INPUT_MAX_CONTROLLERS   3       // todo, dumb limit...
#define INPUT_MAX_VIBRATION     65535   // todo, make configurable?
#define INPUT_MAX_ZONE          32767   // todo, make configurable
#define INPUT_MIN_ZONE          3000    // todo, make configurable

enum {
    PL_KEY_BS       = 8,
    PL_KEY_TAB      = 9,
    PL_KEY_CR       = 13,
    PL_KEY_ESCAPE   = 27,
    PL_KEY_SPACE    = 32,
    PL_KEY_DELETE   = 127,

    PL_KEY_F1,
    PL_KEY_F2,
    PL_KEY_F3,
    PL_KEY_F4,
    PL_KEY_F5,
    PL_KEY_F6,
    PL_KEY_F7,
    PL_KEY_F8,
    PL_KEY_F9,
    PL_KEY_F10,
    PL_KEY_F11,
    PL_KEY_F12,

    PL_KEY_PAUSE,

    PL_KEY_UP,
    PL_KEY_DOWN,
    PL_KEY_LEFT,
    PL_KEY_RIGHT,

    PL_KEY_CTRL,
    PL_KEY_ALT,
    PL_KEY_SHIFT,

    PL_KEY_INSERT,
    PL_KEY_HOME,
    PL_KEY_END,
    PL_KEY_PAGEUP,
    PL_KEY_PAGEDOWN,

    PL_KEY_PRINTSCREEN,

    PL_MOUSE_LEFT = 0,
    PL_MOUSE_MIDDLE,
    PL_MOUSE_RIGHT,
};

/////////////////////////////////////////////////////////////////////////////////////

struct {
    bool controller_support;

    struct {
#if defined(PL_USE_SDL2)
        SDL_Joystick *joystick;
        SDL_GameController *pad;
#endif

        bool is_active;
        float left_magnitude, right_magnitude;
    } controller[INPUT_MAX_CONTROLLERS];

    // Mouse
    int cursor_position[2];
    bool mouse_state[3]; // left, middle, right

    // Keyboard
    bool key_state[256];
} _pl_input;

/////////////////////////////////////////////////////////////////////////////////////
// KEYBOARD

void plClearKeyboardState(void) {
    memset(&_pl_input.key_state, 0, sizeof(bool) * 256);
}

/* Returns true if pressed, false otherwise */
bool plGetKeyState(char key) {
    return _pl_input.key_state[key];
}

// MOUSE

/////////////////////////////////////////////////////////////////////////////////////

void _plInitInput(void) {
    memset(&_pl_input, 0, sizeof(_pl_input));

#if defined(PL_USE_SDL2)
    if(SDL_WasInit(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) == 0) {
        _pl_input.controller_support = true;

        SDL_JoystickEventState(true);
        SDL_GameControllerEventState(true);

        for(unsigned int i = 0; i < SDL_NumJoysticks(); ++i) {
            if(SDL_IsGameController(i)) {
                if((_pl_input.controller[i].pad = SDL_GameControllerOpen(i))) {
                    _pl_input.controller[i].is_active = true;
                }
            } else {
                if((_pl_input.controller[i].joystick = SDL_JoystickOpen(i))) {
                    _pl_input.controller[i].is_active = true;
                }
            }
        }
    } else {
        _plDebugPrint("Failed to initialise controller support!\n%s", SDL_GetError());
    }
#endif
}

void _plShutdownInput(void) {
#if 0 // we are initializing all the subsystems now instead...
#if defined(PL_USE_SDL2)
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
#endif
#endif
}

#if defined(PL_USE_SDL2)
char _plConvertSDL2Key(int key) {
    switch (key) {
        case SDLK_PAGEUP:		return PL_KEY_PAGEUP;
        case SDLK_PAGEDOWN:		return PL_KEY_PAGEDOWN;
        case SDLK_PRINTSCREEN:  return PL_KEY_PRINTSCREEN;
        case SDLK_HOME:			return PL_KEY_HOME;
        case SDLK_END:			return PL_KEY_END;
        case SDLK_LEFT:			return PL_KEY_LEFT;
        case SDLK_RIGHT:		return PL_KEY_RIGHT;
        case SDLK_DOWN:			return PL_KEY_DOWN;
        case SDLK_UP:			return PL_KEY_UP;
        case SDLK_ESCAPE:		return PL_KEY_ESCAPE;
        case SDLK_KP_SPACE:
        case SDLK_SPACE:        return PL_KEY_SPACE;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:		return PL_KEY_CR;
        case SDLK_TAB:			return PL_KEY_TAB;

        case SDLK_F1:			return PL_KEY_F1;
        case SDLK_F2:			return PL_KEY_F2;
        case SDLK_F3:			return PL_KEY_F3;
        case SDLK_F4:			return PL_KEY_F4;
        case SDLK_F5:			return PL_KEY_F5;
        case SDLK_F6:			return PL_KEY_F6;
        case SDLK_F7:			return PL_KEY_F7;
        case SDLK_F8:			return PL_KEY_F8;
        case SDLK_F9:			return PL_KEY_F9;
        case SDLK_F10:			return PL_KEY_F10;
        case SDLK_F11:			return PL_KEY_F11;
        case SDLK_F12:			return PL_KEY_F12;

        case SDLK_DELETE:		return PL_KEY_DELETE;
        case SDLK_BACKSPACE:	return PL_KEY_BS;
        case SDLK_PAUSE:		return PL_KEY_PAUSE;
        //case SDLK_CAPSLOCK:		return K_CAPSLOCK;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:		return PL_KEY_SHIFT;
        case SDLK_LCTRL:
        case SDLK_RCTRL:		return PL_KEY_CTRL;
        case SDLK_LALT:
        case SDLK_RALT:			return PL_KEY_ALT;
        case SDLK_INSERT:		return PL_KEY_INSERT;

        case SDLK_KP_0:	{
            if(SDL_GetModState() & KMOD_NUM) {
                return PL_KEY_INSERT;
            }
            return '0';
        }
        case SDLK_KP_1: {
            if(SDL_GetModState() & KMOD_NUM) {
                return PL_KEY_END;
            }
            return '1';
        }
        case SDLK_KP_2: {
            if(SDL_GetModState() & KMOD_NUM) {
                return PL_KEY_DOWN;
            }
            return '2';
        }
        case SDLK_KP_3:	{
            if(SDL_GetModState() & KMOD_NUM) {
                return PL_KEY_PAGEDOWN;
            }
            return '3';
        }
        case SDLK_KP_4:			return '4';
        case SDLK_KP_5:			return '5';
        case SDLK_KP_6:			return '6';
        case SDLK_KP_7:			return '7';
        case SDLK_KP_8:			return '8';
        case SDLK_KP_9:			return '9';

        case SDLK_KP_MULTIPLY:	return '*';
        case SDLK_KP_PLUS:		return '+';
        case SDLK_KP_MINUS:		return '-';
        case SDLK_KP_DIVIDE:	return '/';

        default:    			return (char)key;
    }
}
#endif

void(*InputKeyboardCallback)(bool state, char key);
void(*InputMouseCallback)(bool state, char button, int x, int y);

void _plInputFrame(void) {
#if defined(PL_USE_SDL2)
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_WINDOWEVENT: {
                switch(event.window.event) {
                    default: {
                        // todo, pass this onto window sub system...
                        break;
                    }

                    case SDL_WINDOWEVENT_FOCUS_LOST:
                    case SDL_WINDOWEVENT_FOCUS_GAINED: {
                        plClearKeyboardState();
                        break;
                    }
                }
            }

            /*  Keyboard Input  */

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                char key = _plConvertSDL2Key(event.key.keysym.sym);
                _pl_input.key_state[key] = (event.key.state == SDL_PRESSED);
                if(InputKeyboardCallback) {
                    InputKeyboardCallback(
                            _pl_input.key_state[key],
                            key
                    );
                }
                break;
            }

            /*  Mouse Input */

            case SDL_MOUSEMOTION: {
                _pl_input.cursor_position[0] = event.motion.x;
                _pl_input.cursor_position[1] = event.motion.y;
                if(InputMouseCallback) {
                    InputMouseCallback(
                            false, -1,
                            _pl_input.cursor_position[0],
                            _pl_input.cursor_position[1]
                    );
                }
                break;
            }

            case SDL_MOUSEWHEEL: {
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                if(event.button.button > SDL_BUTTON_RIGHT) {
                    break;
                }
                _pl_input.mouse_state[event.button.button] = (event.key.state == SDL_PRESSED);
                if(InputMouseCallback) {
                    InputMouseCallback(
                            _pl_input.mouse_state[event.button.button],
                            event.button.button,
                            _pl_input.cursor_position[0],
                            _pl_input.cursor_position[1]
                    );
                }
                break;
            }

            /*  Controller Input    */

            /*  Joystick Input  */

            case SDL_JOYAXISMOTION:
            case SDL_JOYHATMOTION: {
                break;
            }

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP: {
                break;
            }

            default:
                break;
        }
    }
#endif
}

/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_events.h>

#include "engine.h"
#include "input.h"

struct {
    struct {
        bool    key_states[INPUT_MAX_KEYS];
        int     bindings[INPUT_MAX_ACTIONS];
    } keyboard;

    struct {
        int     x, y;
        bool    button_states[INPUT_MAX_MOUSE_BUTTONS];
    } mouse;

    struct {
        bool                button_states[INPUT_MAX_KEYS];
        int                 bindings[INPUT_MAX_ACTIONS];
        SDL_GameController  *pnt;
    } controllers[INPUT_MAX_CONTROLLERS];

    unsigned int num_joysticks;

    void(*InputFocusCallback)(int key, bool is_pressed);
    void(*InputTextCallback)(const char* c);
} input_state;

void Input_Initialize(void) {
    memset(&input_state, 0, sizeof(input_state));

    /* setup the default keyboard bindings */
    input_state.keyboard.bindings[ACTION_MOVE_FORWARD]  = PORK_KEY_UP;
    input_state.keyboard.bindings[ACTION_MOVE_BACKWARD] = PORK_KEY_DOWN;
    input_state.keyboard.bindings[ACTION_TURN_LEFT]     = PORK_KEY_LEFT;
    input_state.keyboard.bindings[ACTION_TURN_RIGHT]    = PORK_KEY_RIGHT;

    /* ask sdl how many joysticks / controllers we have */
    int num_joysticks = SDL_NumJoysticks();
    if(num_joysticks < 0) {
        LogWarn("%s\n", SDL_GetError());
    } else {
        input_state.num_joysticks = (unsigned int) num_joysticks;
        if(input_state.num_joysticks >= INPUT_MAX_CONTROLLERS) {
            input_state.num_joysticks = INPUT_MAX_CONTROLLERS;
        }

        if(SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt") == -1) {
            LogWarn("%s\n", SDL_GetError());
        } else {
            for (unsigned int i = 0; i < input_state.num_joysticks; ++i) {
                if (SDL_IsGameController(i)) {
                    input_state.controllers[i].pnt = SDL_GameControllerOpen(i);
                    if (input_state.controllers[i].pnt == NULL) {
                        LogWarn("%s\n", SDL_GetError());
                    } else {
                        const char *name = SDL_GameControllerName(input_state.controllers[i].pnt);
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

    /* setup the default controller bindings */
    for(unsigned int i = 0; i < INPUT_MAX_CONTROLLERS; ++i) {
        input_state.controllers[i].bindings[ACTION_MOVE_FORWARD]    = PORK_BUTTON_UP;
        input_state.controllers[i].bindings[ACTION_MOVE_BACKWARD]   = PORK_BUTTON_DOWN;
        input_state.controllers[i].bindings[ACTION_TURN_LEFT]       = PORK_BUTTON_LEFT;
        input_state.controllers[i].bindings[ACTION_TURN_RIGHT]      = PORK_BUTTON_RIGHT;

        input_state.controllers[i].bindings[ACTION_AIM]         = PORK_BUTTON_L1;
        input_state.controllers[i].bindings[ACTION_AIM_UP]      = PORK_BUTTON_R1;
        input_state.controllers[i].bindings[ACTION_AIM_DOWN]    = PORK_BUTTON_R2;

        input_state.controllers[i].bindings[ACTION_PAUSE]   = PORK_BUTTON_START;

        input_state.controllers[i].bindings[ACTION_FIRE]    = PORK_BUTTON_CROSS;
        input_state.controllers[i].bindings[ACTION_JUMP]    = PORK_BUTTON_SQUARE;
    }

    /* todo, cv_input_kb_config, 'KEY_UP;KEY_DOWN;KEY_LEFT;KEY_RIGHT' follows
     * the order of the action enum? Parsed on any update to config and then
     * updates the above through callback...
     */
}

void Input_Shutdown(void) {
    for(unsigned int i = 0; i < INPUT_MAX_CONTROLLERS; ++i) {
        if (input_state.controllers[i].pnt != NULL) {
            SDL_GameControllerClose(input_state.controllers[i].pnt);
            input_state.controllers[i].pnt = NULL;
        }
    }
}

void Input_ResetStates(void) {
    memset(input_state.keyboard.key_states, 0, INPUT_MAX_KEYS);
    memset(input_state.mouse.button_states, 0, INPUT_MAX_MOUSE_BUTTONS);
}

void Input_SetKeyboardFocusCallback(void(*Callback)(int key, bool is_pressed)) {
    input_state.InputFocusCallback = Callback;
}

void Input_SetTextFocusCallback(void(*Callback)(const char* c)) {
    input_state.InputTextCallback = Callback;
}

bool Input_GetKeyState(int key) {
    u_assert(key < INPUT_MAX_KEYS);
    return input_state.keyboard.key_states[key];
}

bool Input_GetButtonState(unsigned int controller, int button) {
    u_assert(controller < INPUT_MAX_CONTROLLERS && button < PORK_MAX_BUTTONS);
    return input_state.controllers[controller].button_states[button];
}

bool Input_GetActionState(unsigned int controller, int action) {
    u_assert(controller < INPUT_MAX_CONTROLLERS);
    return (Input_GetButtonState(controller, input_state.controllers[controller].bindings[action]) ||
            Input_GetKeyState(input_state.keyboard.bindings[action]));
}

void Input_SetButtonState(unsigned int controller, int button, bool status) {
    u_assert(controller < INPUT_MAX_CONTROLLERS && button < PORK_MAX_BUTTONS);
    input_state.controllers[controller].button_states[button] = status;
}

void Input_SetKeyState(int key, bool status) {
    u_assert(key < INPUT_MAX_KEYS);
    if(input_state.InputFocusCallback) {
        input_state.InputFocusCallback(key, status);
    }

    input_state.keyboard.key_states[key] = status;
}

void Input_SetMouseState(int x, int y, int button, bool status) {
    u_assert(button < INPUT_MAX_MOUSE_BUTTONS);
    input_state.mouse.x = x;
    input_state.mouse.y = y;
    if(button >= 0) {
        input_state.mouse.button_states[button] = status;
    }
}

void Input_AddTextCharacter(const char* c) {
    if(input_state.InputTextCallback == NULL) {
        return;
    }

    input_state.InputTextCallback(c);
}

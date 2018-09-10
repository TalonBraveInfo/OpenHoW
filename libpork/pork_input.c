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
#include "pork_engine.h"
#include "pork_input.h"

struct {
    struct {
        bool key_states[PORK_MAX_KEYS];
        int bindings[MAX_ACTIONS];
    } keyboard;

    struct {
        int x, y;
        bool button_states[PORK_MAX_MOUSE_BUTTONS];
    } mouse;

    struct {
        bool button_states[PORK_MAX_KEYS];
        int bindings[MAX_ACTIONS];
    } controllers[PORK_MAX_CONTROLLERS];

    void(*InputFocusCallback)(int key, bool is_pressed);
} input_state;

void InitInput(void) {
    memset(&input_state, 0, sizeof(input_state));

    /* setup the default keyboard bindings */
    input_state.keyboard.bindings[ACTION_MOVE_FORWARD]  = PORK_KEY_UP;
    input_state.keyboard.bindings[ACTION_MOVE_BACKWARD] = PORK_KEY_DOWN;
    input_state.keyboard.bindings[ACTION_TURN_LEFT]     = PORK_KEY_LEFT;
    input_state.keyboard.bindings[ACTION_TURN_RIGHT]    = PORK_KEY_RIGHT;

    /* setup the default controller bindings */
    for(unsigned int i = 0; i < PORK_MAX_CONTROLLERS; ++i) {
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

void ResetInputStates(void) {
    memset(input_state.keyboard.key_states, 0, PORK_MAX_KEYS);
    memset(input_state.mouse.button_states, 0, PORK_MAX_MOUSE_BUTTONS);
}

void SetKeyboardFocusCallback(void(*Callback)(int key, bool is_pressed)) {
    input_state.InputFocusCallback = Callback;
}

bool GetKeyState(int key) {
    pork_assert(key < PORK_MAX_KEYS);
    return input_state.keyboard.key_states[key];
}

bool GetButtonState(unsigned int controller, int button) {
    pork_assert(controller < PORK_MAX_CONTROLLERS && button < PORK_MAX_BUTTONS);
    return input_state.controllers[controller].button_states[button];
}

bool GetActionState(unsigned int controller, int action) {
    pork_assert(controller < PORK_MAX_CONTROLLERS);
    return (GetButtonState(controller, input_state.controllers[controller].bindings[action]) ||
            GetKeyState(input_state.keyboard.bindings[action]));
}

/* public interface ****************************************************************/

void SetButtonState(unsigned int controller, int button, bool status) {
    pork_assert(controller < PORK_MAX_CONTROLLERS && button < PORK_MAX_BUTTONS);
    input_state.controllers[controller].button_states[button] = status;
}

void SetKeyState(int key, bool status) {
    pork_assert(key < PORK_MAX_KEYS);
    if(input_state.InputFocusCallback) {
        input_state.InputFocusCallback(key, status);
    }

    input_state.keyboard.key_states[key] = status;
}

void SetMouseState(int x, int y, int button, bool status) {

}
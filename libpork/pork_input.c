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
} input_target;

void InitInput(void) {
    memset(&input_target, 0, sizeof(input_target));

    /* setup the default keyboard bindings */
    input_target.keyboard.bindings[ACTION_MOVE_FORWARD]  = PORK_KEY_UP;
    input_target.keyboard.bindings[ACTION_MOVE_BACKWARD] = PORK_KEY_DOWN;
    input_target.keyboard.bindings[ACTION_MOVE_LEFT]     = PORK_KEY_LEFT;
    input_target.keyboard.bindings[ACTION_MOVE_RIGHT]    = PORK_KEY_RIGHT;

    /* setup the default controller bindings */
    for(unsigned int i = 0; i < PORK_MAX_CONTROLLERS; ++i) {
        input_target.controllers[i].bindings[ACTION_MOVE_FORWARD]    = PORK_BUTTON_UP;
        input_target.controllers[i].bindings[ACTION_MOVE_BACKWARD]   = PORK_BUTTON_DOWN;
        input_target.controllers[i].bindings[ACTION_MOVE_LEFT]       = PORK_BUTTON_LEFT;
        input_target.controllers[i].bindings[ACTION_MOVE_RIGHT]      = PORK_BUTTON_RIGHT;
    }

    /* todo, cv_input_kb_config, 'KEY_UP;KEY_DOWN;KEY_LEFT;KEY_RIGHT' follows
     * the order of the action enum? Parsed on any update to config and then
     * updates the above through callback...
     */
}

void ResetInputStates(void) {
    memset(input_target.keyboard.key_states, 0, PORK_MAX_KEYS);
    memset(input_target.mouse.button_states, 0, PORK_MAX_MOUSE_BUTTONS);
}

void SetKeyboardFocusCallback(void(*Callback)(int key, bool is_pressed)) {
    input_target.InputFocusCallback = Callback;
}

bool GetKeyState(int key) {
    assert(key < PORK_MAX_KEYS);
    return input_target.keyboard.key_states[key];
}

bool GetButtonState(unsigned int controller, int button) {
    assert(controller < PORK_MAX_CONTROLLERS && button < PORK_MAX_BUTTONS);
    return input_target.controllers[controller].button_states[button];
}

bool GetActionState(unsigned int controller, int action) {
    assert(controller < PORK_MAX_CONTROLLERS);
    return (GetButtonState(controller, input_target.controllers[controller].bindings[action]) ||
            GetKeyState(input_target.keyboard.bindings[action]));
}

/* public interface ****************************************************************/

void SetPorkButtonState(unsigned int controller, int button, bool status) {
    assert(controller < PORK_MAX_CONTROLLERS && button < PORK_MAX_BUTTONS);
    input_target.controllers[controller].button_states[button] = status;
}

void SetPorkKeyState(int key, bool status) {
    assert(key < PORK_MAX_KEYS);
    if(input_target.InputFocusCallback) {
        input_target.InputFocusCallback(key, status);
    }

    input_target.keyboard.key_states[key] = status;
}

void SetPorkMouseState(int x, int y, int button, bool status) {

}
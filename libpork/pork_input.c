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
    } keyboard;

    struct {
        int x, y;
        bool button_states[PORK_MAX_MOUSE_BUTTONS];
    } mouse;

    struct {
        bool button_states[PORK_MAX_KEYS];
    } controllers[PORK_MAX_CONTROLLERS];
} input_state;

void InitInput(void) {
    memset(&input_state, 0, sizeof(input_state));
}

void ResetInputStates(void) {
    memset(input_state.keyboard.key_states, 0, PORK_MAX_KEYS);
    memset(input_state.mouse.button_states, 0, PORK_MAX_MOUSE_BUTTONS);
}

bool GetKeyState(int key) {
    assert(key < PORK_MAX_KEYS);
    return input_state.keyboard.key_states[key];
}

/* public interface ****************************************************************/

void SetPorkButtonState(unsigned int controller, int button, bool status) {
    assert(controller < PORK_MAX_CONTROLLERS && button < PORK_MAX_BUTTONS);
    input_state.controllers[controller].button_states[button] = status;
}

void SetPorkKeyState(int key, bool status) {
    assert(key < PORK_MAX_KEYS);
    input_state.keyboard.key_states[key] = status;
}

void SetPorkMouseState(int x, int y, int button, bool status) {

}
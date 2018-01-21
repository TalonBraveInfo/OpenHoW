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
    bool key_states[END_KEY];
} input_state;

void InitInput(void) {
    memset(&input_state, 0, sizeof(input_state));
}

void ResetKeyboardState(void) {
    memset(input_state.key_states, 0, END_KEY);
}

bool GetKeyState(int key) {
    if(key >= END_KEY) {
        LogWarn("passed invalid input key %d, aborting\n", key);
        return false;
    }

    return input_state.key_states[key];
}

/* interface with host */

void PorkKeyboardInput(int key, bool status) {
    if(key >= END_KEY) {
        LogWarn("passed invalid input key %d, aborting\n", key);
        return;
    }

    input_state.key_states[key] = status;
}

void PorkMouseInput(int x, int y, int button, bool status) {

}
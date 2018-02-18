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
#pragma once

enum {
    ACTION_MOVE_FORWARD,
    ACTION_MOVE_BACKWARD,
    ACTION_TURN_LEFT,
    ACTION_TURN_RIGHT,

    ACTION_AIM,
    ACTION_AIM_UP,
    ACTION_AIM_DOWN,

    ACTION_JUMP,

    ACTION_PAUSE,

    ACTION_FIRE,

    ACTION_SELECT,
    ACTION_DESELECT,

    MAX_ACTIONS
};

void InitInput(void);
void ResetInputStates(void);

void SetKeyboardFocusCallback(void(*Callback)(int key, bool is_pressed));

bool GetKeyState(int key);
bool GetButtonState(unsigned int controller, int button);
bool GetActionState(unsigned int controller, int action);
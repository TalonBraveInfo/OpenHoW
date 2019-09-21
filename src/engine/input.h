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

#pragma once

#define INPUT_MAX_CONTROLLERS   4

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

  INPUT_MAX_ACTIONS
};

enum {
  INPUT_BUTTON_CROSS,
  INPUT_BUTTON_CIRCLE,
  INPUT_BUTTON_TRIANGLE,
  INPUT_BUTTON_SQUARE,

  INPUT_BUTTON_R1,
  INPUT_BUTTON_R2,
  INPUT_BUTTON_R3,

  INPUT_BUTTON_L1,
  INPUT_BUTTON_L2,
  INPUT_BUTTON_L3,

  INPUT_BUTTON_UP,
  INPUT_BUTTON_DOWN,
  INPUT_BUTTON_LEFT,
  INPUT_BUTTON_RIGHT,

  INPUT_BUTTON_START,
  INPUT_BUTTON_SELECT,

  INPUT_MAX_BUTTONS
};

enum {
  INPUT_MOUSE_BUTTON_LEFT,
  INPUT_MOUSE_BUTTON_RIGHT,
  INPUT_MOUSE_BUTTON_MIDDLE,

  INPUT_MAX_MOUSE_BUTTONS
};

enum {
  INPUT_JOYSTICK_LEFT,
  INPUT_JOYSTICK_RIGHT,
  INPUT_JOYSTICK_TRIGGERS,

  INPUT_MAX_JOYSTICKS
};

enum {
  INPUT_KEY_TAB = 9,

  INPUT_KEY_F1 = 128,
  INPUT_KEY_F2,
  INPUT_KEY_F3,
  INPUT_KEY_F4,
  INPUT_KEY_F5,
  INPUT_KEY_F6,
  INPUT_KEY_F7,
  INPUT_KEY_F8,
  INPUT_KEY_F9,
  INPUT_KEY_F10,
  INPUT_KEY_F11,
  INPUT_KEY_F12,

  INPUT_KEY_PAUSE,
  INPUT_KEY_INSERT,
  INPUT_KEY_HOME,

  INPUT_KEY_UP,
  INPUT_KEY_DOWN,
  INPUT_KEY_LEFT,
  INPUT_KEY_RIGHT,

  INPUT_KEY_SPACE,

  INPUT_KEY_PAGEUP,
  INPUT_KEY_PAGEDOWN,

  INPUT_KEY_LCTRL,
  INPUT_KEY_LSHIFT,
  INPUT_KEY_LALT,
  INPUT_KEY_RCTRL,
  INPUT_KEY_RSHIFT,
  INPUT_KEY_RALT,

  INPUT_KEY_ESCAPE,

  INPUT_MAX_KEYS
};

PL_EXTERN_C

void Input_Initialize(void);
void Input_ResetStates(void);

void Input_SetKeyboardFocusCallback(void(*Callback)(int key, bool is_pressed));
void Input_SetTextFocusCallback(void(*Callback)(const char *c));

struct PLVector2 Input_GetJoystickState(unsigned int controller, unsigned int joystick);
bool Input_GetKeyState(int key);
bool Input_GetButtonState(unsigned int controller, int button);
bool Input_GetActionState(unsigned int controller, int action);

void Input_SetAxisState(unsigned int controller, unsigned int axis, int status);
void Input_SetButtonState(unsigned int controller, int button, bool status);
void Input_SetKeyState(int key, bool status);
void Input_SetMouseState(int x, int y, int button, bool status);

void Input_AddTextCharacter(const char *c);

PL_EXTERN_C_END

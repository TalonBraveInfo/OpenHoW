/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

namespace ohw {
	class InputManager {
	public:
		InputManager();
		~InputManager();

		enum Key {
			KEY_INVALID = -1,
			KEY_TAB = 9,
			KEY_F1 = 128,
			KEY_F2,
			KEY_F3,
			KEY_F4,
			KEY_F5,
			KEY_F6,
			KEY_F7,
			KEY_F8,
			KEY_F9,
			KEY_F10,
			KEY_F11,
			KEY_F12,
			KEY_PAUSE,
			KEY_INSERT,
			KEY_HOME,
			KEY_UP,
			KEY_DOWN,
			KEY_LEFT,
			KEY_RIGHT,
			KEY_SPACE,
			KEY_PAGEUP,
			KEY_PAGEDOWN,
			KEY_LCTRL,
			KEY_LSHIFT,
			KEY_LALT,
			KEY_RCTRL,
			KEY_RSHIFT,
			KEY_RALT,
			KEY_ESCAPE,
			MAX_KEYS
		};

		enum Action {
			ACTION_INVALID = -1,
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

		enum JoystickAxis {
			JOYSTICK_AXIS_INVALID = -1,
			JOYSTICK_AXIS_LEFT_STICK,
			JOYSTICK_AXIS_RIGHT_STICK,
			JOYSTICK_AXIS_LEFT_TRIGGER,
			JOYSTICK_AXIS_RIGHT_TRIGGER,
			MAX_JOYSTICK_AXIS
		};

		enum MouseButton {
			MOUSE_BUTTON_INVALID = -1,
			MOUSE_BUTTON_LEFT,
			MOUSE_BUTTON_RIGHT,
			MOUSE_BUTTON_MIDDLE,
			MAX_MOUSE_BUTTONS
		};

		void ResetStates();

		void SetKeyboardFocusCallback( void (*Callback)( int input, bool isPressed ) );
		void SetTextFocusCallback( void (*Callback)( const char *c ) );

		bool GetKeyState( int input );
		bool GetJoystickState( unsigned int slot, JoystickAxis input, float *x, float *y );
		bool GetButtonState( unsigned int slot, SDL_GameControllerButton input );
		bool GetActionState( unsigned int slot, Action input );

		void SetAxisState( unsigned int slot, JoystickAxis input, const hei::Vector2 &status );
		void SetButtonState( unsigned int slot, unsigned char input, bool status );
		void SetKeyState( int input, bool status );
		void SetMouseState( int x, int y, MouseButton input, bool status );

		void AddTextCharacter( const char *c );

		void SetupControllers();

	protected:
	private:
		void (*InputFocusCallback)( int input, bool status ){ nullptr };
		void (*InputTextCallback)( const char *c ){ nullptr };

		PLVector2 myMouseCoords;
		bool myMouseButtonStates[MAX_MOUSE_BUTTONS];
		bool myKeyStates[MAX_KEYS];

		struct ActionBinding {
			ActionBinding() {}

			inline void Bind( int k ) {
				// Check it's not been bound already
				auto i = std::find( keys.begin(), keys.end(), k );
				if ( i != keys.end() ) {
					return;
				}

				keys.push_back( k );
			}

			inline void Bind( SDL_GameControllerButton b ) {
				// Check it's not been bound already
				auto i = std::find( buttons.begin(), buttons.end(), b );
				if ( i != buttons.end() ) {
					return;
				}

				buttons.push_back( b );
			}

			inline void Bind( MouseButton b ) {
				// Check it's not been bound already
				auto i = std::find( mouseButtons.begin(), mouseButtons.end(), b );
				if ( i != mouseButtons.end() ) {
					return;
				}

				mouseButtons.push_back( b );
			}

			std::vector< int > keys;
			std::vector< SDL_GameControllerButton > buttons;
			std::vector< MouseButton > mouseButtons;
		};

		ActionBinding myBindings[MAX_ACTIONS];

		struct Controller {
			Controller( unsigned int slot );
			~Controller();

			bool buttonStates[SDL_CONTROLLER_BUTTON_MAX];
			PLVector2 axisStates[MAX_JOYSTICK_AXIS];

			std::string name;
			unsigned int inputSlot;

			void *sdlGameController{ nullptr };
		};

		std::vector< Controller > myControllers;

		inline Controller *GetControllerForSlot( unsigned int slot ) {
			if ( myControllers.empty() ) {
				return nullptr;
			}

			// If the controller doesn't exist, fallback to the first
			if ( slot >= myControllers.size() ) {
				slot = 0;
			}

			return &myControllers[ slot ];
		}
	};
}

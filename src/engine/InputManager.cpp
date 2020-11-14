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

#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_events.h>

#include "App.h"

ohw::InputManager::InputManager() {
	// Setup the default bindings

	myBindings[ ACTION_MOVE_FORWARD ].Bind( KEY_UP );
	myBindings[ ACTION_MOVE_FORWARD ].Bind( 'w' );
	myBindings[ ACTION_MOVE_FORWARD ].Bind( SDL_CONTROLLER_BUTTON_DPAD_DOWN );

	myBindings[ ACTION_MOVE_BACKWARD ].Bind( KEY_DOWN );
	myBindings[ ACTION_MOVE_BACKWARD ].Bind( 's' );
	myBindings[ ACTION_MOVE_BACKWARD ].Bind( SDL_CONTROLLER_BUTTON_DPAD_DOWN );

	myBindings[ ACTION_TURN_LEFT ].Bind( KEY_LEFT );
	myBindings[ ACTION_TURN_LEFT ].Bind( 'a' );
	myBindings[ ACTION_TURN_LEFT ].Bind( SDL_CONTROLLER_BUTTON_DPAD_LEFT );

	myBindings[ ACTION_TURN_RIGHT ].Bind( KEY_RIGHT );
	myBindings[ ACTION_TURN_RIGHT ].Bind( 'd' );
	myBindings[ ACTION_TURN_RIGHT ].Bind( SDL_CONTROLLER_BUTTON_DPAD_RIGHT );

	myBindings[ ACTION_JUMP ].Bind( KEY_SPACE );
	myBindings[ ACTION_JUMP ].Bind( SDL_CONTROLLER_BUTTON_A );

	myBindings[ ACTION_SELECT ].Bind( KEY_TAB );
	myBindings[ ACTION_SELECT ].Bind( SDL_CONTROLLER_BUTTON_B );

	myBindings[ ACTION_AIM ].Bind( KEY_TAB );
	myBindings[ ACTION_AIM ].Bind( SDL_CONTROLLER_BUTTON_LEFTSHOULDER );

	myBindings[ ACTION_AIM_UP ].Bind( KEY_PAGEUP );
	myBindings[ ACTION_AIM_UP ].Bind( 'a' );
	myBindings[ ACTION_AIM_UP ].Bind( SDL_CONTROLLER_BUTTON_RIGHTSHOULDER );

	myBindings[ ACTION_AIM_DOWN ].Bind( KEY_PAGEDOWN );
	myBindings[ ACTION_AIM_UP ].Bind( 'z' );

	myBindings[ ACTION_PAUSE ].Bind( KEY_ESCAPE );
	myBindings[ ACTION_PAUSE ].Bind( SDL_CONTROLLER_BUTTON_START );

	ResetStates();
}

ohw::InputManager::~InputManager() {
	myControllers.clear();
}

void ohw::InputManager::ResetStates() {
	memset( myKeyStates, 0, MAX_KEYS );
	memset( myMouseButtonStates, 0, MAX_MOUSE_BUTTONS );
}

void ohw::InputManager::SetKeyboardFocusCallback( void (*Callback)( int, bool ) ) {
	InputFocusCallback = Callback;
}

void ohw::InputManager::SetTextFocusCallback( void (*Callback)( const char * ) ) {
	InputTextCallback = Callback;
}

bool ohw::InputManager::GetKeyState( int input ) {
	u_assert( input < MAX_KEYS );
	return myKeyStates[ input ];
}

bool ohw::InputManager::GetJoystickState( unsigned int slot, JoystickAxis input, float *x, float *y ) {
	Controller *controller = GetControllerForSlot( slot );
	if ( controller == nullptr ) {
		return false;
	}

	*x = controller->axisStates[ input ].x;
	*y = controller->axisStates[ input ].y;
	return true;
}

bool ohw::InputManager::GetButtonState( unsigned int slot, SDL_GameControllerButton input ) {
	Controller *controller = GetControllerForSlot( slot );
	if ( controller == nullptr ) {
		return false;
	}

	return controller->buttonStates[ input ];
}

bool ohw::InputManager::GetActionState( unsigned int slot, Action input ) {
	for( auto &i : myBindings[ input ].keys ) {
		if( GetKeyState( i ) ) {
			return true;
		}
	}

	if( !myControllers.empty() ) {
		for ( auto &i : myBindings[ input ].buttons ) {
			if ( GetButtonState( slot, i ) ) {
				return true;
			}
		}
	}

	return false;
}

void ohw::InputManager::SetAxisState( unsigned int slot, JoystickAxis input, const PLVector2 &status ) {
	Controller *controller = GetControllerForSlot( slot );
	if ( controller == nullptr ) {
		return;
	}

	// TODO: handle deadzones!!!!

	controller->axisStates[ input ] = status / 100.0f;
}

void ohw::InputManager::SetButtonState( unsigned int slot, unsigned char input, bool status ) {
	if ( input >= SDL_CONTROLLER_BUTTON_MAX ) {
		Warning( "Unhandled controller button %d!\n", input );
		return;
	}

	Controller *controller = GetControllerForSlot( slot );
	if ( controller == nullptr ) {
		return;
	}

	controller->buttonStates[ input ] = status;
}

void ohw::InputManager::SetKeyState( int input, bool status ) {
	u_assert( input <= MAX_KEYS );
	if ( InputFocusCallback != nullptr ) {
		InputFocusCallback( input, status );
	}

	myKeyStates[ input ] = status;
}

void ohw::InputManager::SetMouseState( int x, int y, MouseButton input, bool status ) {
	myMouseCoords = PLVector2( x, y );

	if ( input != MOUSE_BUTTON_INVALID ) {
		myMouseButtonStates[ input ] = status;
	}
}

void ohw::InputManager::AddTextCharacter( const char *c ) {
	if ( InputTextCallback == nullptr ) {
		return;
	}

	InputTextCallback( c );
}

void ohw::InputManager::SetupControllers() {
	SDL_GameControllerEventState( SDL_ENABLE );

	int numJoysticks = SDL_NumJoysticks();
	if ( numJoysticks < 0 ) {
		Warning( "%s\n", SDL_GetError() );
		return;
	}

	if ( SDL_GameControllerAddMappingsFromFile( "gamecontrollerdb.txt" ) == -1 ) {
		Warning( "%s\n", SDL_GetError() );
		return;
	}

	for( int i = 0; i < numJoysticks; ++i ) {
		if ( !SDL_IsGameController( i ) ) {
			continue;
		}

		myControllers.push_back( Controller( i ) );
	}
}

///////////////////////////////////////////
// Controller

ohw::InputManager::Controller::Controller( unsigned int slot ) : inputSlot( slot ) {
	sdlGameController = SDL_GameControllerOpen( slot );
	if( sdlGameController == nullptr ) {
		Warning( "Failed to open controller %d!\nSDL: %s\n", SDL_GetError() );
		return;
	}

	name = SDL_GameControllerNameForIndex( slot );
	if( name.empty() ) {
		name = "unknown";
	}

	Print( "Controller %s\n", name.c_str() );
}

ohw::InputManager::Controller::~Controller() {
	if( sdlGameController != nullptr ) {
		SDL_GameControllerClose( static_cast< SDL_GameController *>( sdlGameController ) );
	}
}

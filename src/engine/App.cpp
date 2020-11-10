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

#include "App.h"
#include "Display.h"
#include "imgui_layer.h"
#include "Language.h"
#include "config.h"
#include "Menu.h"

#define WINDOW_TITLE        "OpenHoW"

ohw::App *appInstance;
ohw::App *ohw::GetApp() {
	return appInstance;
}

static void* u_malloc( size_t size ) {
	return ohw::GetApp()->MAlloc( size, true );
}

static void* u_calloc( size_t num, size_t size ) {
	return ohw::GetApp()->CAlloc( num, size, true );
}

ohw::App::App( int argc, char **argv ) {
	pl_malloc = u_malloc;
	pl_calloc = u_calloc;

	plInitialize( argc, argv );
	plInitializeSubSystems( PL_SUBSYSTEM_IO );

	plRegisterStandardPackageLoaders();
	plRegisterStandardImageLoaders( PL_IMAGE_FILEFORMAT_TGA | PL_IMAGE_FILEFORMAT_PNG | PL_IMAGE_FILEFORMAT_BMP | PL_IMAGE_FILEFORMAT_TIM );

	// Mount the working directory first (./mods/...)
	plMountLocalLocation( plGetWorkingDirectory() );

	char appDataPath[PL_SYSTEM_MAX_PATH];
	plGetApplicationDataDirectory( APP_NAME, appDataPath, PL_SYSTEM_MAX_PATH );

	if ( !plCreatePath( appDataPath ) ) {
		DisplayMessageBox( MBErrorLevel::WARNING_MSG, "Unable to create %s: %s\nSettings will not be saved.", appDataPath, plGetError() );
	}

	// Then mount the app data dir so we can read from the config
	// or load any mods that have been placed under there
	plMountLocalLocation( appDataPath );

	char logPath[ PL_SYSTEM_MAX_PATH ];
	snprintf( logPath, sizeof( logPath ), "%s/debug.txt", appDataPath );
	plSetupLogOutput( logPath );

	plSetupLogLevel( LOG_LEVEL_DEFAULT, "info", PLColour( 0, 255, 0, 255 ), true );
	plSetupLogLevel( LOG_LEVEL_WARNING, "warning", PLColour( 255, 255, 0, 255 ), true );
	plSetupLogLevel( LOG_LEVEL_ERROR, "error", PLColour( 255, 0, 0, 255 ), true );
	plSetupLogLevel( LOG_LEVEL_DEBUG, "debug", PLColour( 0, 255, 255, 255 ),
#ifdef _DEBUG
                     true
#else
			false
#endif
	);

	Print( "Initializing OpenHoW %s\n", GetVersionString() );

	if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) {
		Error( "Failed to initialize SDL2!\nSDL: %s", SDL_GetError() );
	}

	SDL_DisableScreenSaver();

	/* using this to catch modified keys
	 * without having to do the conversion
	 * ourselves                            */
	SDL_StartTextInput();

	resourceManager = new ResourceManager();
	modManager = new ModManager();
	inputManager = new InputManager();
}

void ohw::App::Shutdown() {
	Config_Save( Config_GetUserConfigPath() );

#if 0
	ImGui_ImplOpenGL3_DestroyDeviceObjects();
	ImGui::DestroyContext();
#endif

	delete gameManager;
	delete audioManager;
	delete resourceManager;
	delete myDisplay;

	LanguageManager::DestroyInstance();

	SDL_StopTextInput();

	SDL_EnableScreenSaver();
	SDL_Quit();

	plShutdown();

	exit( EXIT_SUCCESS );
}

void ohw::App::DisplayMessageBox( MBErrorLevel level, const char *message, ... ) {
	unsigned int sdlLevel;
	switch( level ) {
		case MBErrorLevel::INFORMATION_MSG:
			sdlLevel = SDL_MESSAGEBOX_INFORMATION;
			break;
		case MBErrorLevel::WARNING_MSG:
			sdlLevel = SDL_MESSAGEBOX_WARNING;
			break;
		case MBErrorLevel::ERROR_MSG:
			sdlLevel = SDL_MESSAGEBOX_ERROR;
			break;
	}

	va_list args;
	va_start( args, message );

	int length = pl_vscprintf( message, args ) + 1;
	char *buf = static_cast<char *>(MAlloc( length, true ));
	vsprintf( buf, message, args );
	va_end( args );

	SDL_ShowSimpleMessageBox( sdlLevel, WINDOW_TITLE, buf, nullptr );

	delete[] buf;
}

void ohw::App::InitializeConfig() {
	Console_Initialize();

	/* this MUST be done after all vars have been
	 * initialized, otherwise, right now, certain
	 * vars will not be loaded/saved! */
	Config_Load( CONFIG_FILENAME );
}

void ohw::App::InitializeDisplay() {
	const char* arg;
	// check the command line for any arguments
	arg = plGetCommandLineArgumentValue( "-msaa" );
	if ( arg != nullptr ) {
		int i = std::strtol( arg, nullptr, 10 );
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, i );
	}

	int width = 0;
	arg = plGetCommandLineArgumentValue( "-width" );
	if ( arg != nullptr ) {
		width = std::strtol( arg, nullptr, 10 );
	}

	int height = 0;
	arg = plGetCommandLineArgumentValue( "-height" );
	if ( arg != nullptr ) {
		height = std::strtol( arg, nullptr, 10 );
	}

	int desiredDisplay = 0;
	arg = plGetCommandLineArgumentValue( "-display" );
	if ( arg != nullptr ) {
		desiredDisplay = std::strtol( arg, nullptr, 10 );
	}

	myDisplay = new Display( WINDOW_TITLE, width, height, desiredDisplay, true );
	myDisplay->SetIcon( "icon.png" );

	// Fetch all the available display modes
	Print( "Querying display modes...\n" );
	int numModes = SDL_GetNumDisplayModes( desiredDisplay );
	for ( int i = 0; i < numModes; ++i ) {
		SDL_DisplayMode mode;
		if ( SDL_GetDisplayMode( desiredDisplay, i, &mode ) != 0 ) {
			Warning( "Failed to get display mode %d via SDL!\nSDL: %s\n", i, SDL_GetError() );
			continue;
		}

		// Skip any modes that are smaller than our minimum allowed sizes
		if ( mode.w < DISPLAY_MIN_WIDTH || mode.h < DISPLAY_MIN_HEIGHT ) {
			continue;
		}

		//Print( " mode %d : %dx%d %d\n", i, mode.w, mode.h, mode.refresh_rate );
		myDisplayPresets.push_back( DisplayPreset( mode.w, mode.h, mode.refresh_rate ) );
	}

	ImGuiImpl_Setup();
}

void ohw::App::InitializeAudio() {
	audioManager = new AudioManager();
	audioManager->SetupMusicSource();
}

void ohw::App::InitializeGame() {
	// Initialize the language manager
	LanguageManager::GetInstance()->SetLanguage( "eng" );

	gameManager = new GameManager();
	gameManager->CachePersistentData();

	Menu_Initialize();
}

///////////////////////////////////////////////
// Events

static int TranslateSDLKey( int key ) {
	if ( key < 128 ) {
		return key;
	}

	switch ( key ) {
		case SDLK_F1:
			return ohw::InputManager::KEY_F1;
		case SDLK_F2:
			return ohw::InputManager::KEY_F2;
		case SDLK_F3:
			return ohw::InputManager::KEY_F3;
		case SDLK_F4:
			return ohw::InputManager::KEY_F4;
		case SDLK_F5:
			return ohw::InputManager::KEY_F5;
		case SDLK_F6:
			return ohw::InputManager::KEY_F6;
		case SDLK_F7:
			return ohw::InputManager::KEY_F7;
		case SDLK_F8:
			return ohw::InputManager::KEY_F8;
		case SDLK_F9:
			return ohw::InputManager::KEY_F9;
		case SDLK_F10:
			return ohw::InputManager::KEY_F10;
		case SDLK_F11:
			return ohw::InputManager::KEY_F11;
		case SDLK_F12:
			return ohw::InputManager::KEY_F12;

		case SDLK_ESCAPE:
			return ohw::InputManager::KEY_ESCAPE;

		case SDLK_PAUSE:
			return ohw::InputManager::KEY_PAUSE;
		case SDLK_INSERT:
			return ohw::InputManager::KEY_INSERT;
		case SDLK_HOME:
			return ohw::InputManager::KEY_HOME;

		case SDLK_UP:
			return ohw::InputManager::KEY_UP;
		case SDLK_DOWN:
			return ohw::InputManager::KEY_DOWN;
		case SDLK_LEFT:
			return ohw::InputManager::KEY_LEFT;
		case SDLK_RIGHT:
			return ohw::InputManager::KEY_RIGHT;

		case SDLK_SPACE:
			return ohw::InputManager::KEY_SPACE;

		case SDLK_LSHIFT:
			return ohw::InputManager::KEY_LSHIFT;
		case SDLK_RSHIFT:
			return ohw::InputManager::KEY_RSHIFT;

		case SDLK_PAGEUP:
			return ohw::InputManager::KEY_PAGEUP;
		case SDLK_PAGEDOWN:
			return ohw::InputManager::KEY_PAGEDOWN;

		default:
			return -1;
	}
}

static ohw::InputManager::MouseButton TranslateSDLMouseButton( int button ) {
	switch ( button ) {
		case SDL_BUTTON_LEFT:
			return ohw::InputManager::MOUSE_BUTTON_LEFT;
		case SDL_BUTTON_RIGHT:
			return ohw::InputManager::MOUSE_BUTTON_RIGHT;
		case SDL_BUTTON_MIDDLE:
			return ohw::InputManager::MOUSE_BUTTON_MIDDLE;
		default:
			return ohw::InputManager::MOUSE_BUTTON_INVALID;
	}
}

void ohw::App::PollEvents() {
	SDL_Event event;
	while ( SDL_PollEvent( &event ) ) {
		// Check if ImGui wants to handle the event first
		if ( ImGuiImpl_HandleEvent( event ) || myDisplay->HandleEvent( event ) ) {
			continue;
		}

		switch ( event.type ) {
			default:break;

			case SDL_KEYUP:
			case SDL_KEYDOWN: {
				int key = TranslateSDLKey( event.key.keysym.sym );
				if ( key != -1 ) {
					inputManager->SetKeyState( key, ( event.type == SDL_KEYDOWN ) );
				}
				break;
			}

			case SDL_TEXTINPUT:
				inputManager->AddTextCharacter( event.text.text );
				break;

			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN: {
				ohw::InputManager::MouseButton button = TranslateSDLMouseButton( event.button.button );
				inputManager->SetMouseState( event.motion.x, event.motion.y, button, event.button.state );
				break;
			}

			case SDL_MOUSEMOTION: {
				inputManager->SetMouseState( event.motion.x, event.motion.y, ohw::InputManager::MOUSE_BUTTON_INVALID, false );
				break;
			}

			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP: {
				inputManager->SetButtonState( ( unsigned int ) event.cbutton.which, event.cbutton.button, ( event.type == SDL_CONTROLLERBUTTONDOWN ) );
				break;
			}

			case SDL_CONTROLLERAXISMOTION:
#if 0 // TODO: return to this later
				if ( event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.value > 1000 ) {
					inputManager->SetButtonState( ( unsigned int ) event.caxis.which, INPUT_BUTTON_L2, true );
				} else if ( event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.value <= 1000 ) {
					inputManager->SetButtonState( ( unsigned int ) event.caxis.which, INPUT_BUTTON_L2, false );
				}

				if ( event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && event.caxis.value > 1000 ) {
					inputManager->SetButtonState( ( unsigned int ) event.caxis.which, INPUT_BUTTON_R2, true );
				} else if ( event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && event.caxis.value <= 1000 ) {
					inputManager->SetButtonState( ( unsigned int ) event.caxis.which, INPUT_BUTTON_R2, false );
				}

				inputManager->SetAxisState( event.caxis.which, event.caxis.axis, event.caxis.value );
#endif
				break;

			case SDL_QUIT: {
				Shutdown();
				break;
			}
		}
	}
}

///////////////////////////////////////////////
// System

const char *ohw::App::GetVersionString() {
	const char *version = "v"
			PL_TOSTRING( APP_MAJOR_VERSION ) "."
			PL_TOSTRING( APP_MINOR_VERSION ) "."
			PL_TOSTRING( APP_PATCH_VERSION ) "-"
			GIT_BRANCH ":" GIT_COMMIT_HASH "-" GIT_COMMIT_COUNT;

	return version;
}

bool ohw::App::IsRunning() {
	// Clear all the profiling timers
	myPerformanceTimers.clear();

	PollEvents();

	static unsigned int nextTick = 0;
	if ( nextTick == 0 ) {
		nextTick = SDL_GetTicks();
	}

	unsigned int loops = 0;
	while ( SDL_GetTicks() > nextTick && loops < MAX_FRAMESKIP ) {
		numSysTicks = SDL_GetTicks();
		numSimTicks++;

		gameManager->Tick();
		audioManager->Tick();

		lastSysTick = SDL_GetTicks();
		nextTick += SKIP_TICKS;
		loops++;
	}

	deltaTime = ( double ) ( SDL_GetTicks() + SKIP_TICKS - nextTick ) / ( double ) ( SKIP_TICKS );

	myDisplay->Render( deltaTime );

	return true;
}

void *ohw::App::MAlloc( size_t size, bool abortOnFail ) {
	return CAlloc( 1, size, abortOnFail );
}

void *ohw::App::CAlloc( size_t num, size_t size, bool abortOnFail ) {
	void *mem = calloc( num, size );
	if ( mem == nullptr && abortOnFail ) {
		Error( "Failed to allocate %u bytes!\n", size * num );
	}

	return mem;
}

const char *ohw::App::GetClipboardText( void * ) {
	static char* clipboard = nullptr;
	if ( clipboard != nullptr ) {
		SDL_free( clipboard );
	}

	clipboard = SDL_GetClipboardText();
	return clipboard;
}

void ohw::App::SetClipboardText( void *, const char *text ) {
	SDL_SetClipboardText( text );
}

//////////////////////////////////////////////////////
// PROFILING

void ohw::App::StartPerformanceTimer( const char *identifier ) {
	myPerformanceTimers.insert( std::pair< std::string, Timer >( identifier, Timer() ) );
}

void ohw::App::EndPerformanceTimer( const char *identifier ) {
	auto i = myPerformanceTimers.find( identifier );
	if ( i == myPerformanceTimers.end() ) {
		Warning( "Attempted to end an invalid timer, \"%s\"!\n", identifier );
		return;
	}

	i->second.End();
}

int main( int argc, char** argv ) {
#if defined( _DEBUG )
	setvbuf( stdout, nullptr, _IONBF, 0 );
#endif

	appInstance = new ohw::App( argc, argv );

	// Check the mod here, so we can add our VFS paths.
	const char *var = plGetCommandLineArgumentValue( "-mod" );
	if ( var == nullptr ) {
		// otherwise default to base campaign
		var = "how";
	}
	appInstance->modManager->Mount( var );

	appInstance->InitializeConfig();
	appInstance->InitializeDisplay();
	appInstance->InitializeAudio();
	appInstance->InitializeGame();

	while ( appInstance->IsRunning() );

	appInstance->Shutdown();

	return EXIT_SUCCESS;
}

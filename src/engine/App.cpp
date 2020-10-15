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

#include <SDL2/SDL.h>

#include "App.h"
#include "ModManager.h"
#include "input.h"

#define APP_NAME    "OpenHoW"

#define WINDOW_TITLE        "OpenHoW"
#define WINDOW_MIN_WIDTH    640
#define WINDOW_MIN_HEIGHT   480

static ohw::App *appInstance;
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
		DisplayMessageBox( MBErrorLevel::WARNING, "Unable to create %s: %s\nSettings will not be saved.", appDataPath, plGetError() );
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

	if ( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) {
		Error( "Failed to initialize SDL2!\nSDL: %s", SDL_GetError() );
	}

	/* using this to catch modified keys
	 * without having to do the conversion
	 * ourselves                            */
	SDL_StartTextInput();

	// Setup the mod manager and mount the default mod
	myModManager = new ModManager();
	const char *var = plGetCommandLineArgumentValue( "-mod" );
	if ( var == nullptr ) {
		// otherwise default to base campaign
		var = "how";
	}
	myModManager->Mount( var );

	plParseConsoleString( "fsListMounted" );
}

void ohw::App::Shutdown() {
#if 0
	ImGui_ImplOpenGL3_DestroyDeviceObjects();
	ImGui::DestroyContext();
#endif

	SDL_StopTextInput();

	if ( myGLContext != nullptr ) {
		SDL_GL_DeleteContext( myGLContext );
	}

	if ( myWindow != nullptr ) {
		SDL_DestroyWindow( myWindow );
	}

	SDL_ShowCursor( SDL_TRUE );

	SDL_EnableScreenSaver();
	SDL_Quit();

	plShutdown();

	exit( EXIT_SUCCESS );
}

void ohw::App::DisplayMessageBox( MBErrorLevel level, const char *message, ... ) {
	unsigned int sdlLevel;
	switch( level ) {
		case MBErrorLevel::INFORMATION:
			sdlLevel = SDL_MESSAGEBOX_INFORMATION;
			break;
		case MBErrorLevel::WARNING:
			sdlLevel = SDL_MESSAGEBOX_WARNING;
			break;
		case MBErrorLevel::ERROR:
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

///////////////////////////////////////////////
// Display

void ohw::App::InitializeDisplay() {
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

	const char* arg = plGetCommandLineArgumentValue( "-msaa" );
	if ( arg != nullptr ) {
		int i = std::strtol( arg, nullptr, 10 );
		SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, i );
	}

#ifdef _DEBUG
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );

	// Check if the user is specifying what display they want to use
	arg = plGetCommandLineArgumentValue( "-display" );
	if ( arg != nullptr ) {
		myDesiredDisplay = std::strtol( arg, nullptr, 10 );
	}

	// Fetch the display size
	SDL_Rect displayBounds;
	SDL_GetDisplayBounds( myDesiredDisplay, &displayBounds );
	int width = displayBounds.w;
	int height = displayBounds.h;

	myWindow = SDL_CreateWindow(
			WINDOW_TITLE,
			SDL_WINDOWPOS_CENTERED_DISPLAY( myDesiredDisplay ),
			SDL_WINDOWPOS_CENTERED_DISPLAY( myDesiredDisplay ),
			width, height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS );
	if ( myWindow == nullptr ) {
		Error( "Failed to create window!\nSDL: %s\n", SDL_GetError() );
	}

	SetWindowIcon( "icon.png" );

	SDL_SetWindowMinimumSize( myWindow, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT );

	// Now create the GL context
	myGLContext = SDL_GL_CreateContext( myWindow );
	if ( myGLContext == nullptr ) {
		Error( "Failed to create OpenGL context!\nSDL: %s\n", SDL_GetError() );
	}

	// platform library graphics subsystem can init now
	plInitializeSubSystems( PL_SUBSYSTEM_GRAPHICS );
	plSetGraphicsMode( PL_GFX_MODE_OPENGL_CORE );

	// Fetch all the available display modes
	int numModes = SDL_GetNumDisplayModes( myDesiredDisplay );
	for ( unsigned int i = 0; i < numModes; ++i ) {
		SDL_DisplayMode mode;
		if ( SDL_GetDisplayMode( myDesiredDisplay, i, &mode ) != 0 ) {
			Warning( "Failed to get display mode %d via SDL!\nSDL: %s\n", i, SDL_GetError() );
			continue;
		}

		Print( " mode %d : %dx%d %d\n", i, mode.w, mode.h, mode.refresh_rate );

		myDisplayPresets.push_back( DisplayPreset( mode.w, mode.h, mode.refresh_rate ) );
	}
	Print( "%s display modes available\n", numModes );

	SDL_DisableScreenSaver();

	//SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse( SDL_TRUE );
	SDL_ShowCursor( SDL_TRUE );
}

void ohw::App::SwapDisplay() {
	SDL_GL_SwapWindow( myWindow );
}

int ohw::App::SetSwapInterval( int interval ) {
	if ( SDL_GL_SetSwapInterval( interval ) != 0 ) {
		Warning( "Failed to set desired swap interval (%d)!\n", interval );
	}

	return SDL_GL_GetSwapInterval();
}

void ohw::App::SetDisplaySize( int width, int height, bool fullscreen ) {
	plSetConsoleVariable( cv_display_fullscreen, fullscreen ? "true" : "false" );

	if ( fullscreen ) {
		// Fetch the display size
		SDL_Rect displayBounds;
		SDL_GetDisplayBounds( myDesiredDisplay, &displayBounds );
		width = displayBounds.w;
		height = displayBounds.h;

		SDL_SetWindowBordered( myWindow, SDL_FALSE );
	} else {
		SDL_SetWindowBordered( myWindow, SDL_TRUE );
	}

	SDL_SetWindowSize( myWindow, width, height );
	SDL_SetWindowPosition( myWindow,
	                       SDL_WINDOWPOS_CENTERED_DISPLAY( myDesiredDisplay ),
	                       SDL_WINDOWPOS_CENTERED_DISPLAY( myDesiredDisplay ) );

	plSetConsoleVariable( cv_display_width, std::to_string( width ).c_str() );
	plSetConsoleVariable( cv_display_height, std::to_string( height ).c_str() );
}

void ohw::App::GetDisplaySize( int *width, int *height ) {
	SDL_GL_GetDrawableSize( myWindow, width, height );
}

///////////////////////////////////////////////
// Events

static int TranslateSDLKey( int key ) {
	if ( key < 128 ) {
		return key;
	}

	switch ( key ) {
		case SDLK_F1:
			return INPUT_KEY_F1;
		case SDLK_F2:
			return INPUT_KEY_F2;
		case SDLK_F3:
			return INPUT_KEY_F3;
		case SDLK_F4:
			return INPUT_KEY_F4;
		case SDLK_F5:
			return INPUT_KEY_F5;
		case SDLK_F6:
			return INPUT_KEY_F6;
		case SDLK_F7:
			return INPUT_KEY_F7;
		case SDLK_F8:
			return INPUT_KEY_F8;
		case SDLK_F9:
			return INPUT_KEY_F9;
		case SDLK_F10:
			return INPUT_KEY_F10;
		case SDLK_F11:
			return INPUT_KEY_F11;
		case SDLK_F12:
			return INPUT_KEY_F12;

		case SDLK_ESCAPE:
			return INPUT_KEY_ESCAPE;

		case SDLK_PAUSE:
			return INPUT_KEY_PAUSE;
		case SDLK_INSERT:
			return INPUT_KEY_INSERT;
		case SDLK_HOME:
			return INPUT_KEY_HOME;

		case SDLK_UP:
			return INPUT_KEY_UP;
		case SDLK_DOWN:
			return INPUT_KEY_DOWN;
		case SDLK_LEFT:
			return INPUT_KEY_LEFT;
		case SDLK_RIGHT:
			return INPUT_KEY_RIGHT;

		case SDLK_SPACE:
			return INPUT_KEY_SPACE;

		case SDLK_LSHIFT:
			return INPUT_KEY_LSHIFT;
		case SDLK_RSHIFT:
			return INPUT_KEY_RSHIFT;

		case SDLK_PAGEUP:
			return INPUT_KEY_PAGEUP;
		case SDLK_PAGEDOWN:
			return INPUT_KEY_PAGEDOWN;

		default:
			return -1;
	}
}

static int TranslateSDLMouseButton( int button ) {
	switch ( button ) {
		case SDL_BUTTON_LEFT:
			return INPUT_MOUSE_BUTTON_LEFT;
		case SDL_BUTTON_RIGHT:
			return INPUT_MOUSE_BUTTON_RIGHT;
		case SDL_BUTTON_MIDDLE:
			return INPUT_MOUSE_BUTTON_MIDDLE;
		default:
			return -1;
	}
}

static int TranslateSDLButton( int button ) {
	switch ( button ) {
		case SDL_CONTROLLER_BUTTON_A:
			return INPUT_BUTTON_CROSS;
		case SDL_CONTROLLER_BUTTON_B:
			return INPUT_BUTTON_CIRCLE;
		case SDL_CONTROLLER_BUTTON_X:
			return INPUT_BUTTON_SQUARE;
		case SDL_CONTROLLER_BUTTON_Y:
			return INPUT_BUTTON_TRIANGLE;

		case SDL_CONTROLLER_BUTTON_BACK:
			return INPUT_BUTTON_SELECT;
		case SDL_CONTROLLER_BUTTON_START:
			return INPUT_BUTTON_START;

		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
			return INPUT_BUTTON_L1;
		case SDL_CONTROLLER_BUTTON_LEFTSTICK:
			return INPUT_BUTTON_L3;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			return INPUT_BUTTON_R1;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
			return INPUT_BUTTON_R3;

		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			return INPUT_BUTTON_DOWN;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			return INPUT_BUTTON_LEFT;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			return INPUT_BUTTON_RIGHT;
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
			return INPUT_BUTTON_UP;

		default:
			return -1;
	}
}

void ohw::App::PollEvents() {
	ImGuiIO& io = ImGui::GetIO();

	SDL_Event event;
	while ( SDL_PollEvent( &event ) ) {
		switch ( event.type ) {
			default:break;

			case SDL_KEYUP:
			case SDL_KEYDOWN: {
				// Always update if key-up; see
				// https://github.com/TalonBraveInfo/OpenHoW/issues/70#issuecomment-507377604
				if ( event.type == SDL_KEYUP || io.WantCaptureKeyboard ) {
					int key = event.key.keysym.scancode;
					IM_ASSERT( key >= 0 && key < IM_ARRAYSIZE( io.KeysDown ) );
					io.KeysDown[ key ] = ( event.type == SDL_KEYDOWN );
					io.KeyShift = ( ( SDL_GetModState() & KMOD_SHIFT ) != 0 );
					io.KeyCtrl = ( ( SDL_GetModState() & KMOD_CTRL ) != 0 );
					io.KeyAlt = ( ( SDL_GetModState() & KMOD_ALT ) != 0 );
					io.KeySuper = ( ( SDL_GetModState() & KMOD_GUI ) != 0 );
					if ( event.type != SDL_KEYUP ) break;
				}

				int key = TranslateSDLKey( event.key.keysym.sym );
				if ( key != -1 ) {
					Input_SetKeyState( key, ( event.type == SDL_KEYDOWN ) );
				}
				break;
			}

			case SDL_TEXTINPUT:
				if ( io.WantCaptureKeyboard ) {
					io.AddInputCharactersUTF8( event.text.text );
					break;
				}

				Input_AddTextCharacter( event.text.text );
				break;

			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN: {
				if ( io.WantCaptureMouse ) {
					switch ( event.button.button ) {
						case SDL_BUTTON_LEFT:io.MouseDown[ 0 ] = event.button.state;
							break;
						case SDL_BUTTON_RIGHT:io.MouseDown[ 1 ] = event.button.state;
							break;
						case SDL_BUTTON_MIDDLE:io.MouseDown[ 2 ] = event.button.state;
							break;

						default:break;
					}

					break;
				}

				int button = TranslateSDLMouseButton( event.button.button );
				Input_SetMouseState( event.motion.x, event.motion.y, button, event.button.state );
				break;
			}

			case SDL_MOUSEWHEEL:
				if ( event.wheel.x > 0 ) {
					io.MouseWheelH += 1;
				} else if ( event.wheel.x < 0 ) {
					io.MouseWheelH -= 1;
				}

				if ( event.wheel.y > 0 ) {
					io.MouseWheel += 1;
				} else if ( event.wheel.y < 0 ) {
					io.MouseWheel -= 1;
				}
				break;

			case SDL_MOUSEMOTION:io.MousePos.x = event.motion.x;
				io.MousePos.y = event.motion.y;

				Input_SetMouseState( event.motion.x, event.motion.y, -1, false );
				break;

			case SDL_CONTROLLERBUTTONUP: {
				int button = TranslateSDLButton( event.cbutton.button );
				if ( button != -1 ) {
					Input_SetButtonState( ( unsigned int ) event.cbutton.which, button, false );
				}
				break;
			}
			case SDL_CONTROLLERBUTTONDOWN: {
				int button = TranslateSDLButton( event.cbutton.button );
				if ( button != -1 ) {
					Input_SetButtonState( ( unsigned int ) event.cbutton.which, button, true );
				}
				break;
			}

			case SDL_CONTROLLERAXISMOTION:
				if ( event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.value > 1000 ) {
					Input_SetButtonState( ( unsigned int ) event.caxis.which, INPUT_BUTTON_L2, true );
				} else if ( event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT && event.caxis.value <= 1000 ) {
					Input_SetButtonState( ( unsigned int ) event.caxis.which, INPUT_BUTTON_L2, false );
				}

				if ( event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && event.caxis.value > 1000 ) {
					Input_SetButtonState( ( unsigned int ) event.caxis.which, INPUT_BUTTON_R2, true );
				} else if ( event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT && event.caxis.value <= 1000 ) {
					Input_SetButtonState( ( unsigned int ) event.caxis.which, INPUT_BUTTON_R2, false );
				}

				Input_SetAxisState( event.caxis.which, event.caxis.axis, event.caxis.value );
				break;

			case SDL_QUIT:
				Shutdown();
				break;

			case SDL_WINDOWEVENT: {
				if ( event.window.event == SDL_WINDOWEVENT_RESIZED ||
				     event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) {
					char buf[16];
					plSetConsoleVariable( cv_display_width, pl_itoa( event.window.data1, buf, 16, 10 ) );
					plSetConsoleVariable( cv_display_height, pl_itoa( event.window.data2, buf, 16, 10 ) );
					Display_UpdateViewport( 0, 0, event.window.data1, event.window.data2 );
					ImGuiImpl_UpdateViewport( event.window.data1, event.window.data2 );
					io.DisplaySize = ImVec2( event.window.data1, event.window.data2 );
				}
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
#define TICKS_PER_SECOND    25
#define SKIP_TICKS          (1000 / TICKS_PER_SECOND)
#define MAX_FRAMESKIP       5

	PollEvents();

	static unsigned int nextTick = 0;
	if ( nextTick == 0 ) {
		nextTick = SDL_GetTicks();
	}

	unsigned int loops = 0;
	while ( SDL_GetTicks() > nextTick && loops < MAX_FRAMESKIP ) {
		numSysTicks = SDL_GetTicks();
		numSimTicks++;

#if 0
		Client_ProcessInput(); // todo: kill this
		Physics()->Tick();
		Game()->Tick();
		Audio()->Tick();
#endif

		lastSysTick = SDL_GetTicks();
		nextTick += SKIP_TICKS;
		loops++;
	}

	deltaTime = ( double ) ( SDL_GetTicks() + SKIP_TICKS - nextTick ) / ( double ) ( SKIP_TICKS );
	Display_Draw( deltaTime );

	return true;
}

void *ohw::App::MAlloc( size_t size, bool abortOnFail ) {
	CAlloc( 1, size, abortOnFail );
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

void ohw::App::SetWindowIcon( const char *path ) {
	PLImage *image = plLoadImage( path );
	if ( image == nullptr ) {
		Warning( "Failed to load image, %s!\nPL: %s\n", path, plGetError() );
		return;
	}

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
			image->data[ 0 ],
			// Casting casting casting, why o why
			static_cast<signed>( image->width ),
			static_cast<signed>( image->height ),
			32,
			static_cast<signed>( image->width * 4 ),
			0x000000ff,
			0x0000ff00,
			0x00ff0000,
			0xff000000
	);
	if ( surface == nullptr ) {
		Warning( "Failed to create requested SDL surface!\nSDL: %s\n", SDL_GetError() );
	} else {
		SDL_SetWindowIcon( myWindow, surface );
	}

	SDL_FreeSurface( surface );

	plDestroyImage( image );
}

int main( int argc, char** argv ) {
#if defined( _DEBUG )
	setvbuf( stdout, nullptr, _IONBF, 0 );
#endif

	appInstance = new ohw::App( argc, argv );
	appInstance->InitializeDisplay();

	while ( appInstance->IsRunning() );

	appInstance->Shutdown();

	return EXIT_SUCCESS;
}

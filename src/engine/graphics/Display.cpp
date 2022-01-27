// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include <plgraphics/plg_driver_interface.h>

#include "App.h"
#include "imgui_layer.h"
#include "Menu.h"
#include "Map.h"
#include "ShaderManager.h"
#include "Camera.h"

#include "game/ActorManager.h"
#include "Display.h"

/************************************************************/

void Display_GetFramesCount( unsigned int *fps, unsigned int *ms ) {
	static unsigned int fps_ = 0;
	static unsigned int ms_ = 0;
	//static unsigned int update_delay = 60;
#if 0 // todo: revisit
	if ( update_delay < g_state.draw_ticks && g_state.last_draw_ms > 0 ) {
		ms_ = g_state.last_draw_ms;
		fps_ = 1000 / ms_;
		update_delay = g_state.draw_ticks + 60;
	}
#endif

	*fps = fps_;
	*ms = ms_;
}

static void Display_DrawMap() {
	ohw::Map *map = ohw::GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	map->Draw();
}

ohw::Display::Display( const char *title, int w, int h, unsigned int desiredScreen, bool fullscreen ) :
		myDesiredScreen( desiredScreen ) {
	// Fetch the display size.
	SDL_Rect displayBounds;
	SDL_GetDisplayBounds( myDesiredScreen, &displayBounds );
	if ( fullscreen || w < DISPLAY_MIN_WIDTH || h < DISPLAY_MIN_HEIGHT ) {
		w = displayBounds.w;
		h = displayBounds.h;
	}

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
#ifdef _DEBUG
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
#else
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
#endif
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );

	int defaultFlags =
			SDL_WINDOW_OPENGL |
			SDL_WINDOW_MOUSE_FOCUS |
			SDL_WINDOW_INPUT_FOCUS |
			SDL_WINDOW_ALLOW_HIGHDPI;
	// SDL_WINDOW_ALLOW_HIGHDPI currently does nothing on Windows,
	// see https://bugzilla.libsdl.org/show_bug.cgi?id=3281
	// Assume borderless if it matches desktop resolution.
	if ( w == displayBounds.w && h == displayBounds.h ) {
		defaultFlags |= SDL_WINDOW_BORDERLESS;
	}

	myWindow = SDL_CreateWindow(
			title,
			SDL_WINDOWPOS_CENTERED_DISPLAY( myDesiredScreen ),
			SDL_WINDOWPOS_CENTERED_DISPLAY( myDesiredScreen ),
			w, h,
			defaultFlags );
	if ( myWindow == nullptr ) {
		Error( "Failed to create window!\nSDL: %s\n", SDL_GetError() );
	}

	SDL_SetWindowMinimumSize( myWindow, DISPLAY_MIN_WIDTH, DISPLAY_MIN_HEIGHT );

	//SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse( SDL_TRUE );

	// Now create the GL context
	myGLContext = SDL_GL_CreateContext( myWindow );
	if ( myGLContext == nullptr ) {
		Error( "Failed to create OpenGL context!\nSDL: %s\n", SDL_GetError() );
	}

	// platform library graphics subsystem can init now
	if ( PlInitializeSubSystems( PL_SUBSYSTEM_GRAPHICS ) != PL_RESULT_SUCCESS ||
	     PlgInitializeGraphics() != PL_RESULT_SUCCESS ) {
		Error( "Failed to initialize platform graphics subsystem!\nPL: %s\n", PlGetError() );
	}

	PlgScanForDrivers( "." );

	unsigned int numDrivers, i;
	const char **drivers = PlgGetAvailableDriverInterfaces( &numDrivers );
	for ( i = 0; i < numDrivers; ++i ) {
		if ( PlgSetDriver( drivers[ i ] ) != PL_RESULT_SUCCESS ) {
			continue;
		}

		break;
	}

	if ( i == numDrivers ) {
		Error( "Failed to load a valid graphics layer!\n" );
	}

	Shaders_Initialize();
}

ohw::Display::~Display() {
	Shaders_Shutdown();

	PlgShutdownGraphics();

	SDL_CaptureMouse( SDL_FALSE );

	if ( myGLContext != nullptr ) {
		SDL_GL_DeleteContext( myGLContext );
	}
	if ( myWindow != nullptr ) {
		SDL_DestroyWindow( myWindow );
	}
}

void ohw::Display::SetIcon( const char *path ) {
	PLImage *image = PlLoadImage( path );
	if ( image == nullptr ) {
		Warning( "Failed to load image, %s!\nPL: %s\n", path, PlGetError() );
		return;
	}

	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
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
	if ( surface != nullptr ) {
		SDL_SetWindowIcon( myWindow, surface );
		SDL_FreeSurface( surface );
	} else {
		Warning( "Failed to create requested SDL surface!\nSDL: %s\n", SDL_GetError() );
	}

	PlDestroyImage( image );
}

void ohw::Display::Render( double delta ) {
	START_MEASURE();

	numDrawTicks = GetApp()->GetTicks();

	PlgSetCullMode( PLG_CULL_POSTIVE );

	PlgSetDepthMask( true );
	PlgSetDepthBufferMode( PLG_DEPTHBUFFER_ENABLE );

	PlgBindFrameBuffer( nullptr, PLG_FRAMEBUFFER_DRAW );

	PlgSetClearColour( { 0, 0, 0, 255 } );
	PlgClearBuffers( PLG_BUFFER_DEPTH | PLG_BUFFER_COLOUR );

	RenderScene();
	RenderOverlays();

	END_MEASURE();

	RenderDebugOverlays();

	Swap();
}

void ohw::Display::RenderScene() {
	ohw::Camera *camera = ohw::GetApp()->gameManager->GetActiveCamera();
	if ( camera == nullptr ) {
		return;
	}

	START_MEASURE();

	int w, h;
	GetDisplaySize( &w, &h );
	camera->SetViewport( 0, 0, w, h );

	// Sync the camera state before we make it active
	camera->SetFieldOfView( cv_camera_fov->f_value );
	camera->SetNear( cv_camera_near->f_value );
	camera->SetFar( cv_camera_far->f_value );

	camera->MakeActive();

	if ( cv_graphics_alpha_to_coverage->b_value ) {
		PlgEnableGraphicsState( PLG_GFX_STATE_ALPHATOCOVERAGE );
	}

	Display_DrawMap();

	ActorManager::GetInstance()->DrawActors();

	if ( cv_graphics_alpha_to_coverage->b_value ) {
		PlgDisableGraphicsState( PLG_GFX_STATE_ALPHATOCOVERAGE );
	}

	RenderSceneDebug();

	END_MEASURE();
}

void ohw::Display::RenderSceneDebug() {
	Shaders_SetProgramByName( "generic_untextured" );

	ohw::GetApp()->audioManager->DrawSources();

	PlPushMatrix();
	PlMatrixMode( PL_MODELVIEW_MATRIX );
	PlLoadIdentityMatrix();

	for ( const auto &i: debugLines ) {
		PlgDrawSimpleLine( *PlGetMatrix( PL_MODELVIEW_MATRIX ), i.start, i.end, i.colour );
	}
	debugLines.clear();

	PlPopMatrix();
}

void ohw::Display::RenderOverlays() {
	START_MEASURE();

	Menu_Draw();

	END_MEASURE();
}

void ohw::Display::RenderDebugOverlays() {
	if ( cv_imgui->i_value <= 0 ) {
		return;
	}

	ImGuiImpl_SetupFrame();

	UI_DisplayDebugMenu(); /* aka imgui */

	ImGuiImpl_Draw();
}

void ohw::Display::SetDisplaySize( int w, int h, bool fullscreen ) {
	if ( fullscreen ) {
		// Fetch the display size
		SDL_Rect displayBounds;
		SDL_GetDisplayBounds( myDesiredScreen, &displayBounds );
		w = displayBounds.w;
		h = displayBounds.h;

		SDL_SetWindowBordered( myWindow, SDL_FALSE );
	} else {
		SDL_SetWindowBordered( myWindow, SDL_TRUE );
	}

	SDL_SetWindowSize( myWindow, w, h );
	SDL_SetWindowPosition( myWindow,
	                       SDL_WINDOWPOS_CENTERED_DISPLAY( myDesiredScreen ),
	                       SDL_WINDOWPOS_CENTERED_DISPLAY( myDesiredScreen ) );

	PlSetConsoleVariable( cv_display_width, std::to_string( w ).c_str() );
	PlSetConsoleVariable( cv_display_height, std::to_string( h ).c_str() );
	PlSetConsoleVariable( cv_display_fullscreen, fullscreen ? "1" : "0" );
}

void ohw::Display::GetDisplaySize( int *w, int *h ) {
	SDL_GL_GetDrawableSize( myWindow, w, h );
}

void ohw::Display::Swap() {
	SDL_GL_SwapWindow( myWindow );

	// Best place to do this?
	lastDrawMS = GetApp()->GetTicks() - numDrawTicks;
}

int ohw::Display::SetSwapInterval( int desiredInterval ) {
	if ( SDL_GL_SetSwapInterval( desiredInterval ) != 0 ) {
		Warning( "Failed to set desired swap interval \"%d\"!\n", desiredInterval );
	}

	return SDL_GL_GetSwapInterval();
}

bool ohw::Display::HandleEvent( const SDL_Event &event ) {
	switch ( event.type ) {
		default:
			break;
		case SDL_WINDOWEVENT: {
			switch ( event.window.type ) {
				default:
					break;
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					PlSetConsoleVariable( cv_display_width, std::to_string( event.window.data1 ).c_str() );
					PlSetConsoleVariable( cv_display_height, std::to_string( event.window.data2 ).c_str() );
					return true;
			}
			break;
		}
	}

	return false;
}

void ohw::Display::SetMousePosition( int x, int y ) {
	SDL_WarpMouseInWindow( myWindow, x, y );
}

// Debugging

void ohw::Display::DebugDrawLine( const PLVector3 &startPos, const PLVector3 &endPos, const PLColour &colour ) {
	DebugLine debugLine;
	debugLine.start = startPos;
	debugLine.end = endPos;
	debugLine.colour = colour;
	debugLines.push_back( debugLine );
}

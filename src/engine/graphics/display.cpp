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

#include "engine.h"
#include "input.h"
#include "imgui_layer.h"
#include "frontend.h"
#include "Map.h"

#include "game/actor_manager.h"

#include "shaders.h"
#include "display.h"
#include "camera.h"

/* shared function */
void Display_UpdateViewport( int x, int y, int width, int height ) {
	Input_ResetStates();

	if ( g_state.ui_camera == nullptr ) {
		// display probably hasn't been initialised
		return;
	}

	float current_aspect = ( float ) width / ( float ) height;
	float target_aspect = 4.0f / 3.0f;
	float relative_width = target_aspect / current_aspect;
	float relative_height = current_aspect / target_aspect;
	relative_width = relative_width > 1.0f ? 1.0f : relative_width;
	relative_height = relative_height > 1.0f ? 1.0f : relative_height;

	int newWidth = ( float ) width * relative_width;
	int newHeight = ( float ) height * relative_height;

	//TODO: Only adjust viewport aspect of ingame camera once ingame scene is working. Force UI camera to 4:3 viewport always.
	//      For now, just use the same viewport aspect for both.

	if ( FrontEnd_GetState() == FE_MODE_GAME || cv_display_use_window_aspect->b_value ) {
		//If enabled, use full window for 3d scene
		g_state.ui_camera->viewport.x = x;
		g_state.ui_camera->viewport.y = y;
		g_state.ui_camera->viewport.w = width;
		g_state.ui_camera->viewport.h = height;
	} else {
		g_state.ui_camera->viewport.x = ( width - newWidth ) / 2;
		g_state.ui_camera->viewport.y = ( height - newHeight ) / 2;
		g_state.ui_camera->viewport.w = newWidth;
		g_state.ui_camera->viewport.h = newHeight;
	}

	ohw::Camera *camera = ohw::Engine::Game()->GetCamera();
	camera->SetViewport( x, y, width, height );
}

/**
 * Update display to match the current settings.
 */
void Display_UpdateState() {
	char buf[4];

	// bound check both the width and height to lowest supported width and height
	if ( cv_display_width->i_value < MIN_DISPLAY_WIDTH ) {
		plSetConsoleVariable( cv_display_width, pl_itoa( MIN_DISPLAY_WIDTH, buf, 4, 10 ) );
	}
	if ( cv_display_height->i_value < MIN_DISPLAY_HEIGHT ) {
		plSetConsoleVariable( cv_display_height, pl_itoa( MIN_DISPLAY_HEIGHT, buf, 4, 10 ) );
	}

	// attempt to set the new display size, otherwise update cvars to match
	int w = cv_display_width->i_value;
	int h = cv_display_height->i_value;
	if ( !System_SetWindowSize( w, h, false ) ) {
		Warning( "failed to set display size to %dx%d!\n", cv_display_width->i_value, cv_display_height->i_value );
		Print( "display set to %dx%d\n", w, h );

		// ... not sure if we'll force the engine to only render a specific display size ...
		if ( w < MIN_DISPLAY_WIDTH ) w = MIN_DISPLAY_WIDTH;
		if ( h < MIN_DISPLAY_HEIGHT ) h = MIN_DISPLAY_HEIGHT;

		plSetConsoleVariable( cv_display_width, pl_itoa( w, buf, 4, 10 ) );
		plSetConsoleVariable( cv_display_height, pl_itoa( h, buf, 4, 10 ) );
	} else {
		Print( "display set to %dx%d\n", w, h );
	}

	Display_UpdateViewport( 0, 0, cv_display_width->i_value, cv_display_height->i_value );
}

void Display_Initialize() {
	// check the command line for any arguments
	const char *var;
	if ( ( var = plGetCommandLineArgumentValue( "-width" ) ) != nullptr ) {
		plSetConsoleVariable( cv_display_width, var );
	}
	if ( ( var = plGetCommandLineArgumentValue( "-height" ) ) != nullptr ) {
		plSetConsoleVariable( cv_display_height, var );
	}

	if ( plHasCommandLineArgument( "-window" ) ) {
		plSetConsoleVariable( cv_display_fullscreen, "false" );
	} else if ( plHasCommandLineArgument( "-fullscreen" ) ) {
		plSetConsoleVariable( cv_display_fullscreen, "true" );
	}

	// now create the window and update the display
	System_DisplayWindow( true, MIN_DISPLAY_WIDTH, MIN_DISPLAY_HEIGHT );

	System_SetSwapInterval( cv_display_vsync->b_value ? 1 : 0 );

	Display_UpdateState();

	Shaders_Initialize();

	//////////////////////////////////////////////////////////

	// TODO: move into frontend.cpp
	g_state.ui_camera = plCreateCamera();
	if ( g_state.ui_camera == nullptr ) {
		Error( "Failed to create ui camera, aborting!\n%s\n", plGetError() );
	}
	g_state.ui_camera->mode = PL_CAMERA_MODE_ORTHOGRAPHIC;
	g_state.ui_camera->fov = 90;
	g_state.ui_camera->near = 0;
	g_state.ui_camera->far = 1000;
	g_state.ui_camera->viewport.w = cv_display_width->i_value;
	g_state.ui_camera->viewport.h = cv_display_height->i_value;

	ImGuiImpl_SetupCamera();

	plSetCullMode( PL_CULL_POSTIVE );

	plSetDepthMask( true );
}

void Display_Shutdown() {
	Shaders_Shutdown();
}

/************************************************************/

void DEBUGDrawSkeleton();
void DEBUGDrawModel();

void Display_GetFramesCount( unsigned int *fps, unsigned int *ms ) {
	static unsigned int fps_ = 0;
	static unsigned int ms_ = 0;
	static unsigned int update_delay = 60;
	if ( update_delay < g_state.draw_ticks && g_state.last_draw_ms > 0 ) {
		ms_ = g_state.last_draw_ms;
		fps_ = 1000 / ms_;
		update_delay = g_state.draw_ticks + 60;
	}

	*fps = fps_;
	*ms = ms_;
}

static void DrawDebugOverlay() {
	if ( cv_debug_mode->i_value <= 0 ) {
		return;
	}

	UI_DisplayDebugMenu(); /* aka imgui */
}

double cur_delta = 0;

static void Display_DrawMap() {
	ohw::Map *map = ohw::Engine::Game()->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	map->Draw();
}

void Display_DrawScene() {
	ohw::Camera *camera = ohw::Engine::Game()->GetCamera();
	if ( camera == nullptr ) {
		return;
	}

	// Sync the camera state before we make it active
	camera->SetFieldOfView( cv_camera_fov->f_value );
	camera->SetNear( cv_camera_near->f_value );
	camera->SetFar( cv_camera_far->f_value );

	camera->MakeActive();

	if ( cv_graphics_alpha_to_coverage->b_value ) {
		plEnableGraphicsState( PL_GFX_STATE_ALPHATOCOVERAGE );
	}

	Display_DrawMap();

	ActorManager::GetInstance()->DrawActors();

	if ( cv_graphics_alpha_to_coverage->b_value ) {
		plDisableGraphicsState( PL_GFX_STATE_ALPHATOCOVERAGE );
	}

	/* debug methods */

	Shaders_SetProgramByName( "generic_untextured" );

	ohw::Engine::Audio()->DrawSources();
}

void Display_DrawInterface() {
	Shaders_SetProgramByName( "generic_textured" );

	plSetupCamera( g_state.ui_camera );
	plSetDepthBufferMode( PL_DEPTHBUFFER_DISABLE );

	FE_Draw();

	DrawDebugOverlay();
}

void Display_Draw( double delta ) {
	g_state.gfx.numModelsDrawn = 0;

	ImGuiImpl_SetupFrame();

	cur_delta = delta;
	g_state.draw_ticks = System_GetTicks();

	plSetClearColour( PLColour( 0, 0, 0, 255 ) );

	unsigned int clear_flags = PL_BUFFER_DEPTH;
	if ( FrontEnd_GetState() == FE_MODE_GAME || cv_debug_mode->i_value > 0 ) {
		clear_flags |= PL_BUFFER_COLOUR;
	}
	plClearBuffers( clear_flags );

	plSetDepthBufferMode( PL_DEPTHBUFFER_ENABLE );

	plBindFrameBuffer( nullptr, PL_FRAMEBUFFER_DRAW );

	Display_DrawScene();
	Display_DrawInterface();

	ImGuiImpl_Draw();

	Display_Flush();
}

void Display_Flush() {
	System_SwapDisplay();
	g_state.last_draw_ms = System_GetTicks() - g_state.draw_ticks;
}

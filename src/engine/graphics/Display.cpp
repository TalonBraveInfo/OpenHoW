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
#include "imgui_layer.h"
#include "Menu.h"
#include "Map.h"

#include "game/ActorManager.h"

#include "ShaderManager.h"
#include "Display.h"
#include "Camera.h"

/* shared function */
void Display_UpdateViewport( int x, int y, int width, int height ) {
	Menu_UpdateViewport( x, y, width, height );

	ohw::Camera *camera = ohw::GetApp()->gameManager->GetActiveCamera();
	if ( camera == nullptr ) {
		Warning( "Attempted to update the camera viewport with no active camera!\n" );
		return;
	}

	camera->SetViewport( x, y, width, height );
}

void Display_Initialize() {
	ohw::GetApp()->SetSwapInterval( cv_display_vsync->b_value ? 1 : 0 );

	Shaders_Initialize();

	//////////////////////////////////////////////////////////

	plSetCullMode( PL_CULL_POSTIVE );

	plSetDepthMask( true );
}

void Display_Shutdown() {
	Shaders_Shutdown();
}

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

static void DrawDebugOverlay() {
	if ( cv_debug_mode->i_value <= 0 ) {
		return;
	}

	UI_DisplayDebugMenu(); /* aka imgui */
}

double cur_delta = 0;

static void Display_DrawMap() {
	ohw::Map *map = ohw::GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	map->Draw();
}

void Display_DrawScene() {
	ohw::Camera *camera = ohw::GetApp()->gameManager->GetActiveCamera();
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

	ohw::GetApp()->audioManager->DrawSources();
}

void Display_Draw( double delta ) {
	ImGuiImpl_SetupFrame();

	plSetClearColour( PLColour( 0, 0, 0, 255 ) );

	unsigned int clear_flags = PL_BUFFER_DEPTH;
	if ( FrontEnd_GetState() == FE_MODE_GAME || cv_debug_mode->i_value > 0 ) {
		clear_flags |= PL_BUFFER_COLOUR;
	}
	plClearBuffers( clear_flags );

	plSetDepthBufferMode( PL_DEPTHBUFFER_ENABLE );

	plBindFrameBuffer( nullptr, PL_FRAMEBUFFER_DRAW );

	Display_DrawScene();

	Menu_Draw();
	DrawDebugOverlay();

	ImGuiImpl_Draw();

	ohw::GetApp()->SwapDisplay();
}

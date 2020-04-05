/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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

#include <PL/platform_graphics.h>
#include <PL/platform_graphics_camera.h>
#include <PL/platform_filesystem.h>

#include "engine.h"
#include "input.h"
#include "frontend.h"
#include "Map.h"
#include "game/TempGame.h"
#include "graphics/font.h"
#include "graphics/display.h"
#include "graphics/video.h"
#include "graphics/shaders.h"

using namespace openhow;

static unsigned int frontend_state = FE_MODE_INIT;
static unsigned int old_frontend_state = ( unsigned int ) -1;

/* for now we're going to hard-code most of this but eventually
 * we will start freeing most of this up... either through JS
 * or some other way, so y'know. Wheeee.
 */

static const char *papers_teams_paths[MAX_TEAMS] = {
	"frontend/papers/british.bmp",
	"frontend/papers/american.bmp",
	"frontend/papers/french.bmp",
	"frontend/papers/german.bmp",
	"frontend/papers/russian.bmp",
	"frontend/papers/japan.bmp",
	"frontend/papers/teamlard.bmp"
};

/* texture assets, these are loaded and free'd at runtime */
static PLTexture *fe_background = nullptr;
static PLTexture *fe_papers_teams[MAX_TEAMS] = {
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};

enum {
	FE_TEXTURE_ANG,
	FE_TEXTURE_ANGPOINT,

	FE_TEXTURE_CLOCK,
	FE_TEXTURE_TIMER,
	FE_TEXTURE_CLIGHT,

	FE_TEXTURE_CROSSHAIR,
	FE_TEXTURE_TARGET,

	FE_TEXTURE_ARROW,
	FE_TEXTURE_CROSS,

	FE_TEXTURE_PAUSE,

	MAX_FE_GAME_TEXTURES
};
static PLTexture *fe_tx_game_textures[MAX_FE_GAME_TEXTURES];  /* textures that we'll be using in-game */
//static PLTexture *fe_tx_game_icons[MAX_ITEM_TYPES];

static int frontend_width = 0;
static int frontend_height = 0;

/************************************************************/

static void FrontendInputCallback( int key, bool is_pressed ) {
	if ( frontend_state == FE_MODE_START && is_pressed ) {
		/* todo, play 'ting' sound! */

		/* we've hit our key, we can take away this
		 * callback now and carry on to whatever */
		Input_SetKeyboardFocusCallback( nullptr );
		FrontEnd_SetState( FE_MODE_MAIN_MENU );
		return;
	}
}

static void CacheFEGameData() {
	fe_tx_game_textures[ FE_TEXTURE_ANG ] =
		Engine::Resource()->LoadTexture( "frontend/dash/ang", PL_TEXTURE_FILTER_LINEAR, true );
	fe_tx_game_textures[ FE_TEXTURE_ANGPOINT ] =
		Engine::Resource()->LoadTexture( "frontend/dash/angpoint", PL_TEXTURE_FILTER_LINEAR, true );
	fe_tx_game_textures[ FE_TEXTURE_CLOCK ] =
		Engine::Resource()->LoadTexture( "frontend/dash/clock", PL_TEXTURE_FILTER_LINEAR, true );
	fe_tx_game_textures[ FE_TEXTURE_CLIGHT ] =
		Engine::Resource()->LoadTexture( "frontend/dash/timlit.png", PL_TEXTURE_FILTER_LINEAR, true );
	fe_tx_game_textures[ FE_TEXTURE_TIMER ] =
		Engine::Resource()->LoadTexture( "frontend/dash/timer", PL_TEXTURE_FILTER_LINEAR, true );
}

static void CacheFEMenuData() {
	fe_background = Engine::Resource()->LoadTexture( "frontend/pigbkpc1", PL_TEXTURE_FILTER_LINEAR, true );
	for ( unsigned int i = 0; i < MAX_TEAMS; ++i ) {
		fe_papers_teams[ i ] =
			Engine::Resource()->LoadTexture( papers_teams_paths[ i ], PL_TEXTURE_FILTER_LINEAR, true );
	}
}

/************************************************************/

void FE_Initialize( void ) {
	CacheFontData();
	CacheFEMenuData();
	CacheFEGameData();
}

void FE_Shutdown( void ) {
	ClearFontData();
}

void FE_ProcessInput( void ) {
	switch ( frontend_state ) {
		default:break;

		case FE_MODE_START: {
			/* this is... kind of a hack... but ensures that
			 * nothing will take away our check for a key during
			 * the 'start' screen, e.g. bringing the console up */
			Input_SetKeyboardFocusCallback( FrontendInputCallback );
		}
			break;

		case FE_MODE_VIDEO: {
			if ( Input_GetKeyState( INPUT_KEY_SPACE ) || Input_GetKeyState( INPUT_KEY_ESCAPE ) ) {
				Video_SkipCurrent();
			}
		}
			break;
	}
}

void FrontEnd_Tick( void ) {}

/************************************************************/

char loading_description[256];
uint8_t loading_progress = 0;

#if 0
#	define Redraw()   Display_DrawInterface();
#else
#	define Redraw()
#endif

void FE_SetLoadingBackground( const char *name ) {
	char screen_path[PL_SYSTEM_MAX_PATH];
	snprintf( screen_path, sizeof( screen_path ), "frontend/briefing/%s", name );
	if ( !plFileExists( screen_path ) ) {
		snprintf( screen_path, sizeof( screen_path ), "frontend/briefing/loadmult" );
	}

	fe_background = Engine::Resource()->LoadTexture( screen_path, PL_TEXTURE_FILTER_LINEAR );
	Redraw();
}

void FE_SetLoadingDescription( const char *description ) {
	snprintf( loading_description, sizeof( loading_description ), "%s ...", description );
	Redraw();
}

void FE_SetLoadingProgress( uint8_t progress ) {
	if ( progress > 100 ) progress = 100;
	loading_progress = progress;
	Redraw();
}

uint8_t FE_GetLoadingProgress( void ) {
	return loading_progress;
}

/************************************************************/

/**
 * Draw the timer in the bottom corner of the screen.
 */
static void DrawTimer() {
	if ( FrontEnd_GetState() != FE_MODE_GAME ) {
		return;
	}

	IGameMode *mode = Engine::Game()->GetMode();
	if ( !mode->HasRoundStarted() ) {
		return;
	}

	char str[64];
	snprintf( str, sizeof( str ), "%0d / %0d", mode->GetTurnTimeSeconds(), mode->GetMaxTurnTimeSeconds() );
	Font_DrawBitmapString( g_fonts[ FONT_BIG ],
						   frontend_width - 256,
						   frontend_height - 100,
						   4,
						   1.0f,
						   PL_COLOUR_WHITE,
						   str );
}

/**
 * Draw the minimap on the button left corner of the screen.
 */
static void DrawMinimap() {
	if ( FrontEnd_GetState() != FE_MODE_GAME ) {
		return;
	}

	Map *map = openhow::Engine::Game()->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

#if 0
	static PLMesh* pane = nullptr;
	if(pane == nullptr) {
		pane = plNewMeshRectangle(0, 0, 256, 256, PL_COLOUR_WHITE);
		if(pane == nullptr) {
			Error("Failed to create pane mesh!\n");
		}
	}

	plSetTexture(map->GetOverviewTexture(), 0);

	PLMatrix4x4 mat = plMatrix4x4Identity();
	plSetNamedShaderUniformMatrix4x4(NULL, "pl_model", mat, false);

	plUploadMesh(pane);
	plDrawMesh(pane);

	plSetTexture(nullptr, 0);
#else
	PLMatrix4 transform = plMatrix4Identity();

	/* for debugging... */
	unsigned int scr_h = Display_GetViewportHeight( &g_state.ui_camera->viewport );
	plDrawTexturedRectangle( &transform, 0, scr_h - 128, 128, 128, map->GetTerrain()->GetOverview() );
#endif
}

/* Hogs of War's menu was designed
 * with a fixed resolution in mind
 * and scales poorly with anything
 * else. For now we'll just keep
 * things fixed and worry about this
 * later. */

static void DrawLoadingScreen() {
	PLMatrix4 transform = plMatrix4Identity();
	plDrawTexturedRectangle( &transform, 0, 0, frontend_width, frontend_height, fe_background );

	/* originally I wrote some code ensuring the menu bar
	 * was centered... that was until I found out that on
	 * the background, the slot for the bar ISN'T centered
	 * at all. JOY... */
	static const int bar_w = 330;
	int bar_x = 151; //c_x + (FRONTEND_MENU_WIDTH / 2) - bar_w / 2;
	int bar_y = 450;
	if ( loading_progress > 0 ) {
		plDrawFilledRectangle( plCreateRectangle(
			PLVector2( bar_x, bar_y ),
			PLVector2( ( ( float ) ( bar_w ) / 100 ) * loading_progress, 18 ),
			PL_COLOUR_INDIAN_RED,
			PL_COLOUR_INDIAN_RED,
			PL_COLOUR_RED,
			PL_COLOUR_RED
		) );
	}

	if ( loading_description[ 0 ] != ' ' && loading_description[ 0 ] != '\0' ) {
		Font_DrawBitmapString( g_fonts[ FONT_CHARS2 ],
							   bar_x + 2,
							   bar_y + 1,
							   4,
							   1.f,
							   PL_COLOUR_WHITE,
							   loading_description );
	}
}

void FE_Draw( void ) {
	frontend_width = Display_GetViewportWidth( &g_state.ui_camera->viewport );
	frontend_height = Display_GetViewportHeight( &g_state.ui_camera->viewport );

	PLMatrix4 transform = plMatrix4Identity();

	/* render and handle the main menu */
	switch ( frontend_state ) {
		default:break;

		case FE_MODE_INIT:
		case FE_MODE_START:
		case FE_MODE_MAIN_MENU:
			plDrawTexturedRectangle( &transform, 0, 0, frontend_width, frontend_height, fe_background );
			break;

		case FE_MODE_LOADING:
			DrawLoadingScreen();
			break;

		case FE_MODE_VIDEO:
			Video_Draw();
			break;

		case FE_MODE_GAME:
			DrawMinimap();
			DrawTimer();
			break;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * */

void FE_RestoreLastState( void ) {
	FrontEnd_SetState( old_frontend_state );
}

unsigned int FrontEnd_GetState( void ) {
	return frontend_state;
}

void FrontEnd_SetState( unsigned int state ) {
	if ( state == frontend_state ) {
		LogDebug( "attempted to set debug state to an already existing state!\n" );
		return;
	}

	LogDebug( "changing frontend state to %u...\n", state );
	switch ( state ) {
		default: {
			LogWarn( "invalid frontend state, %u, aborting\n", state );
			return;
		}

		case FE_MODE_MAIN_MENU:
			// start playing the default theme
			Engine::Audio()->PlayMusic( AUDIO_MUSIC_MENU );
			break;

		case FE_MODE_START: break;

		case FE_MODE_GAME:
			// game mode handles music from here?
			break;

		case FE_MODE_LOADING: {
			// stop the music as soon as we switch to a loading screen...
			Engine::Audio()->StopMusic();

			loading_description[ 0 ] = '\0';
			loading_progress = 0;
			break;
		}

		case FE_MODE_EDITOR: break;
	}
	old_frontend_state = frontend_state;
	frontend_state = state;

	/* !!hacky!! force the display to update due to aspect change, yeah this is gross... */
	Display_UpdateViewport( 0, 0, cv_display_width->i_value, cv_display_height->i_value );
}


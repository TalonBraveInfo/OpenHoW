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
#include "frontend.h"
#include "Map.h"
#include "graphics/font.h"
#include "graphics/display.h"
#include "graphics/video.h"
#include "game/actor_manager.h"

using namespace ohw;

static unsigned int frontend_state = FE_MODE_INIT;
static unsigned int old_frontend_state = ( unsigned int ) -1;

/* texture assets, these are loaded and free'd at runtime */
static ohw::TextureResource *fe_background = nullptr;
static ohw::TextureResource *minimapIcons[ MAX_MINIMAP_ICONS ];

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

static void FrontEnd_CacheMenuData() {
	fe_background = Engine::Resource()->LoadTexture( "frontend/pigbkpc1", PL_TEXTURE_FILTER_LINEAR, true );
}

/************************************************************/

void FE_Initialize( void ) {
	FrontEnd_CacheFontData();
	FrontEnd_CacheMenuData();

	// Cache all the minimap icons
	minimapIcons[ MINIMAP_ICON_BOMB ]   = Engine::Resource()->LoadTexture( "frontend/map/bomb", PL_TEXTURE_FILTER_NEAREST, true );
	minimapIcons[ MINIMAP_ICON_HEALTH ] = Engine::Resource()->LoadTexture( "frontend/map/iconhart", PL_TEXTURE_FILTER_NEAREST, true );
	minimapIcons[ MINIMAP_ICON_PIG ]    = Engine::Resource()->LoadTexture( "frontend/map/iconpig", PL_TEXTURE_FILTER_NEAREST, true );
	minimapIcons[ MINIMAP_ICON_PICKUP ] = Engine::Resource()->LoadTexture( "frontend/map/iconpkup.png", PL_TEXTURE_FILTER_NEAREST, true );
	minimapIcons[ MINIMAP_ICON_PROP ]   = Engine::Resource()->LoadTexture( "frontend/map/iconprop", PL_TEXTURE_FILTER_NEAREST, true );
}

void FE_Shutdown( void ) {
	ClearFontData();
}

void FE_ProcessInput( void ) {
	switch ( frontend_state ) {
		default:break;

		case FE_MODE_START:
			/* this is... kind of a hack... but ensures that
			 * nothing will take away our check for a key during
			 * the 'start' screen, e.g. bringing the console up */
			Input_SetKeyboardFocusCallback( FrontendInputCallback );
			break;

		case FE_MODE_VIDEO:
			if ( Input_GetKeyState( INPUT_KEY_SPACE ) || Input_GetKeyState( INPUT_KEY_ESCAPE ) ) {
				Video_SkipCurrent();
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
	snprintf( str, sizeof( str ), "%0d", mode->GetMaxTurnTimeSeconds() - mode->GetTurnTimeSeconds() );
	Font_DrawBitmapString( g_fonts[ FONT_BIG ],
						   frontend_width - 256,
						   frontend_height - 100,
						   4,
						   1.0f,
						   PL_COLOUR_WHITE,
						   str );
}

/**
 * Draw the 3D minimap on the bottom left corner of the screen.
 */
static void FrontEnd_DrawMinimap() {
	Map *map = Engine::Game()->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	Camera *camera = Engine::Game()->GetCamera();
	if ( camera == nullptr ) {
		return;
	}

	// So, we need to render the minimap from the perspective here. So swap things round
	PLCamera *uiCamera = g_state.ui_camera;
	PLCamera save = *uiCamera;

	// Set up the camera so we can render the minimap how it needs to be
	uiCamera->mode 			= PL_CAMERA_MODE_PERSPECTIVE;
	uiCamera->near			= 0.1f;
	uiCamera->far			= 100.0f;
	uiCamera->fov			= 65.0f;
	uiCamera->position		= PLVector3( -120.0f, 64.0f, 0.0f );
	uiCamera->angles		= PLVector3( -45.0f, 0.0f, 0.0f );
	uiCamera->viewport.x	= 0;
	uiCamera->viewport.y	= 0;
	uiCamera->viewport.w 	= 450;
	uiCamera->viewport.h 	= 256;

	plSetupCamera( uiCamera );

	PLMatrix4 transform;
	transform.Identity();
	transform.Rotate( plDegreesToRadians( 90.0f ), PLVector3( 1, 0, 0 ) );
	transform.Rotate( plDegreesToRadians( camera->GetAngles().y ), PLVector3( 0, 1, 0 ) );

	plDrawTexturedRectangle( &transform, -64, -64, 128, 128, map->GetTerrain()->GetOverview() );

	// Now draw everything on top
	for ( const auto &actor : ActorManager::GetInstance()->GetActors() ) {
		if ( !actor->IsVisibleOnMinimap() ) {
			continue;
		}

		// Figure out what icon we're using
		PLTexture *iconTexture = Engine::Resource()->GetFallbackTexture();
		unsigned int iconStyle = actor->GetMinimapIconStyle();
		if ( iconStyle < MAX_MINIMAP_ICONS ) {
			iconTexture = minimapIcons[ iconStyle ]->GetInternalTexture();
		}

		plSetTexture( iconTexture, 0 );

		// And now figure out where relatively speaking they should be
		PLVector3 curPosition = actor->GetPosition();
		float x = ( curPosition.x / 256.0f ) - 64.0f;
		float y = ( curPosition.z / 256.0f ) - 64.0f;

		transform.Identity();
		// Angle the plane towards the camera
		transform.Rotate( plDegreesToRadians( 135.0f ), PLVector3( 1, 0, 0 ) );
		// Rotate the yaw so it's always facing the camera
		transform.Rotate( plDegreesToRadians( -camera->GetAngles().y - 90.0f ), PLVector3( 0, 1, 0 ) );
		// And now position and rotate it so it appears on top of the map
		transform.Translate( PLVector3( x, 0.0f, y ) );
		transform.Rotate( plDegreesToRadians( camera->GetAngles().y ), PLVector3( 0, 1, 0 ) );

		// Fetch the transformed distance from the camera
		PLVector3 translation = transform.GetTranslation();
		PLVector3 distance = uiCamera->position - translation;
		float lengthDistance = distance.Length();
		// And now scale it up as it moves further away (this isn't perfect...)
		float scale = 8.0f * lengthDistance / 100.0f;

		plDrawRectangle( &transform, -( scale / 2 ), -( scale / 2 ), scale, scale, actor->GetMinimapIconColour() );
	}

	plSetTexture( nullptr, 0 );

	// Restore what we originally had for the camera
	*uiCamera = save;
	plSetupCamera( uiCamera );
}

/* Hogs of War's menu was designed
 * with a fixed resolution in mind
 * and scales poorly with anything
 * else. For now we'll just keep
 * things fixed and worry about this
 * later. */

static void DrawLoadingScreen() {
	PLMatrix4 transform = plMatrix4Identity();
	plDrawTexturedRectangle( &transform, 0, 0, frontend_width, frontend_height, fe_background->GetInternalTexture() );

	/* originally I wrote some code ensuring the menu bar
	 * was centered... that was until I found out that on
	 * the background, the slot for the bar ISN'T centered
	 * at all. JOY... */
	static const int bar_w = 330;
	int bar_x = 151; //c_x + (FRONTEND_MENU_WIDTH / 2) - bar_w / 2;
	int bar_y = 450;
	if ( loading_progress > 0 ) {
		PLRectangle2D box = plCreateRectangle(
			PLVector2( bar_x, bar_y ),
			PLVector2( ( ( float ) ( bar_w ) / 100 ) * loading_progress, 18 ),
			PL_COLOUR_INDIAN_RED,
			PL_COLOUR_INDIAN_RED,
			PL_COLOUR_RED,
			PL_COLOUR_RED
		);
		plDrawFilledRectangle( &box );
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
			plDrawTexturedRectangle( &transform, 0, 0, frontend_width, frontend_height, fe_background->GetInternalTexture() );
			break;

		case FE_MODE_LOADING:
			DrawLoadingScreen();
			break;

		case FE_MODE_VIDEO:
			Video_Draw();
			break;

		case FE_MODE_GAME:
			FrontEnd_DrawMinimap();
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

		case FE_MODE_START:
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


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
#include "Menu.h"
#include "Map.h"
#include "graphics/display.h"
#include "graphics/video.h"
#include "graphics/Camera.h"
#include "game/ActorManager.h"

static unsigned int frontend_state = FE_MODE_INIT;
static unsigned int old_frontend_state = ( unsigned int ) -1;

/* texture assets, these are loaded and free'd at runtime */
static ohw::TextureResource *menuBackground = nullptr;
static ohw::TextureResource *minimapIcons[MAX_MINIMAP_ICONS];

static int frontend_width = 0;
static int frontend_height = 0;

ohw::BitmapFont *g_fonts[NUM_FONTS];

static PLCamera *menuCamera;

void Menu_Initialize() {
	menuBackground = ohw::GetApp()->resourceManager->LoadTexture( "frontend/pigbkpc1", ohw::TextureResource::FLAG_NOMIPS, true );

	// Cache all the minimap icons
	minimapIcons[ MINIMAP_ICON_BOMB ] =ohw::GetApp()->resourceManager->LoadTexture( "frontend/map/bomb", ohw::TextureResource::FLAG_NOMIPS, true );
	minimapIcons[ MINIMAP_ICON_HEALTH ] =ohw::GetApp()->resourceManager->LoadTexture( "frontend/map/iconhart", ohw::TextureResource::FLAG_NOMIPS, true );
	minimapIcons[ MINIMAP_ICON_PIG ] =ohw::GetApp()->resourceManager->LoadTexture( "frontend/map/iconpig", ohw::TextureResource::FLAG_NOMIPS, true );
	minimapIcons[ MINIMAP_ICON_PICKUP ] =ohw::GetApp()->resourceManager->LoadTexture( "frontend/map/iconpkup.png", ohw::TextureResource::FLAG_NOMIPS, true );
	minimapIcons[ MINIMAP_ICON_PROP ] =ohw::GetApp()->resourceManager->LoadTexture( "frontend/map/iconprop", ohw::TextureResource::FLAG_NOMIPS, true );

	// Cache the default fonts
	struct FontIndex {
		const char *tab, *texture;
	} defaultFonts[NUM_FONTS] = {
			{ "frontend/text/big.tab",       "frontend/text/big.bmp" },
			{ "frontend/text/bigchars.tab",  "frontend/text/bigchars.bmp" },
			{ "frontend/text/chars2.tab",    "frontend/text/chars2l.bmp" },
			{ "frontend/text/chars3.tab",    "frontend/text/chars3.bmp" },
			{ "frontend/text/gamechars.tab", "frontend/text/gamechars.bmp" },
			{ "frontend/text/small.tab",     "frontend/text/small.bmp" },
	};
	for ( unsigned int i = 0; i < NUM_FONTS; ++i ) {
		g_fonts[ i ] = new ohw::BitmapFont();
		if ( !g_fonts[ i ]->Load( defaultFonts[ i ].tab, defaultFonts[ i ].texture ) ) {
			Error( "Failed to load default font!\n" );
		}
	}

	// Setup viewport
	// TODO: move into frontend.cpp
	menuCamera = plCreateCamera();
	if ( menuCamera == nullptr ) {
		Error( "Failed to create ui camera, aborting!\n%s\n", plGetError() );
	}
	menuCamera->mode = PL_CAMERA_MODE_ORTHOGRAPHIC;
	menuCamera->fov = 90;
	menuCamera->near = 0;
	menuCamera->far = 1000;
	menuCamera->viewport.w = cv_display_width->i_value;
	menuCamera->viewport.h = cv_display_height->i_value;
}

void Menu_UpdateViewport( int x, int y, int width, int height ) {
	//TODO: Only adjust viewport aspect of ingame camera once ingame scene is working. Force UI camera to 4:3 viewport always.
	//      For now, just use the same viewport aspect for both.
	
	float current_aspect = ( float ) width / ( float ) height;
	float target_aspect = 4.0f / 3.0f;
	float relative_width = target_aspect / current_aspect;
	float relative_height = current_aspect / target_aspect;
	relative_width = relative_width > 1.0f ? 1.0f : relative_width;
	relative_height = relative_height > 1.0f ? 1.0f : relative_height;

	int newWidth = ( float ) width * relative_width;
	int newHeight = ( float ) height * relative_height;

	if ( FrontEnd_GetState() == FE_MODE_GAME || cv_display_use_window_aspect->b_value ) {
		//If enabled, use full window for 3d scene
		menuCamera->viewport.x = x;
		menuCamera->viewport.y = y;
		menuCamera->viewport.w = width;
		menuCamera->viewport.h = height;
	} else {
		menuCamera->viewport.x = ( width - newWidth ) / 2;
		menuCamera->viewport.y = ( height - newHeight ) / 2;
		menuCamera->viewport.w = newWidth;
		menuCamera->viewport.h = newHeight;
	}
}

/************************************************************/

static void FrontendInputCallback( int key, bool is_pressed ) {
	if ( frontend_state == FE_MODE_START && is_pressed ) {
		/* todo, play 'ting' sound! */

		/* we've hit our key, we can take away this
		 * callback now and carry on to whatever */
		ohw::GetApp()->inputManager->SetKeyboardFocusCallback( nullptr );
		FrontEnd_SetState( FE_MODE_MAIN_MENU );
		return;
	}
}

/************************************************************/

void FE_Shutdown( void ) {
	for ( auto &g_font : g_fonts ) {
		delete g_font;
	}
}

void FE_ProcessInput( void ) {
	switch ( frontend_state ) {
		default:
			break;

		case FE_MODE_START:
			/* this is... kind of a hack... but ensures that
			 * nothing will take away our check for a key during
			 * the 'start' screen, e.g. bringing the console up */
			ohw::GetApp()->inputManager->SetKeyboardFocusCallback( FrontendInputCallback );
			break;

		case FE_MODE_VIDEO:
			if (ohw::GetApp()->inputManager->GetKeyState( ohw::InputManager::KEY_SPACE ) ||ohw::GetApp()->inputManager->GetKeyState( ohw::InputManager::KEY_ESCAPE ) ) {
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

	menuBackground =ohw::GetApp()->resourceManager->LoadTexture( screen_path, ohw::TextureResource::FLAG_NOMIPS );
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

	IGameMode *mode =ohw::GetApp()->gameManager->GetMode();
	if ( !mode->HasRoundStarted() ) {
		return;
	}

	char str[64];
	snprintf( str, sizeof( str ), "%0d", mode->GetMaxTurnTimeSeconds() - mode->GetTurnTimeSeconds() );
	g_fonts[ FONT_BIG ]->DrawString( frontend_width - 256,
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
	ohw::Map *map =ohw::GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	ohw::Camera *camera =ohw::GetApp()->gameManager->GetActiveCamera();
	if ( camera == nullptr ) {
		return;
	}

	// So, we need to render the minimap from the perspective here. So swap things round
	PLCamera *uiCamera = menuCamera;
	PLCamera save = *uiCamera;

	// Set up the camera so we can render the minimap how it needs to be
	uiCamera->mode = PL_CAMERA_MODE_PERSPECTIVE;
	uiCamera->near = 0.1f;
	uiCamera->far = 100.0f;
	uiCamera->fov = 65.0f;
	uiCamera->position = PLVector3( -120.0f, 64.0f, 0.0f );
	uiCamera->angles = PLVector3( -45.0f, 0.0f, 0.0f );
	uiCamera->viewport.x = 0;
	uiCamera->viewport.y = 0;
	uiCamera->viewport.w = 450;
	uiCamera->viewport.h = 256;

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
		PLTexture *iconTexture =ohw::GetApp()->resourceManager->GetFallbackTexture();
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
	plDrawTexturedRectangle( &transform, 0, 0, frontend_width, frontend_height, menuBackground->GetInternalTexture() );

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
		g_fonts[ FONT_CHARS2 ]->DrawString( bar_x + 2,
		                                    bar_y + 1,
		                                    4,
		                                    1.f,
		                                    PL_COLOUR_WHITE,
		                                    loading_description );
	}
}

void Menu_Draw() {
	frontend_width = menuCamera->viewport.w;
	frontend_height = menuCamera->viewport.h;

	PLMatrix4 transform = plMatrix4Identity();

	/* render and handle the main menu */
	switch ( frontend_state ) {
		default:
			break;

		case FE_MODE_INIT:
		case FE_MODE_START:
		case FE_MODE_MAIN_MENU:
			plDrawTexturedRectangle( &transform, 0, 0, frontend_width, frontend_height, menuBackground->GetInternalTexture() );
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
		DebugMsg( "attempted to set debug state to an already existing state!\n" );
		return;
	}

	DebugMsg( "changing frontend state to %u...\n", state );
	switch ( state ) {
		default: {
			Warning( "invalid frontend state, %u, aborting\n", state );
			return;
		}

		case FE_MODE_MAIN_MENU:
			// start playing the default theme
			ohw::GetApp()->audioManager->PlayMusic( AUDIO_MUSIC_MENU );
			break;

		case FE_MODE_START:
		case FE_MODE_GAME:
			// game mode handles music from here?
			break;

		case FE_MODE_LOADING: {
			// stop the music as soon as we switch to a loading screen...
			ohw::GetApp()->audioManager->StopMusic();

			loading_description[ 0 ] = '\0';
			loading_progress = 0;
			break;
		}

		case FE_MODE_EDITOR:
			break;
	}
	old_frontend_state = frontend_state;
	frontend_state = state;

	/* !!hacky!! force the display to update due to aspect change, yeah this is gross... */
	Display_UpdateViewport( 0, 0, cv_display_width->i_value, cv_display_height->i_value );
}


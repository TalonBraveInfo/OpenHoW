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

#include "font.h"
#include "shaders.h"
#include "display.h"
#include "camera.h"

using namespace ohw;

/************************************************************/
/* Texture Cache */

static std::vector<VideoPreset> vidPresets;
//static PLFrameBuffer* game_target;
//static PLFrameBuffer* frontend_target;

static PLConsoleVariable *cv_display_show_camerapos;
static PLConsoleVariable *cv_display_show_viewportinfo;

#if 0
void PrintTextureCacheSizeCommand(unsigned int argc, char *argv[]) {
	size_t cache_size = GetTextureCacheSize();
	const char *str = "total texture cache: ";
	if(argc < 2) {
		LogInfo("%s %u bytes", str, cache_size);
		return;
	}

	if(pl_strncasecmp(argv[1], "KB", 2) == 0) {
		LogInfo("%s %.2fKB (%u bytes)\n", str, plBytesToKilobytes(cache_size), cache_size);
	} else if(pl_strncasecmp(argv[1], "MB", 2) == 0) {
		LogInfo("%s %.2fMB (%u bytes)\n", str, plBytesToMegabytes(cache_size), cache_size);
	} else if(pl_strncasecmp(argv[1], "GB", 2) == 0) {
		LogInfo("%s %.2fGB (%u bytes)\n", str, plBytesToGigabytes(cache_size), cache_size);
	} else {
		LogInfo("unknown parameter \"%s\", ignoring!\n", argv[1]);
	}
}

/* for debugging purposes, displays the textures
 * cached in memory on the screen */
void DrawTextureCache(unsigned int id) {
	u_assert(id < MAX_TEXTURE_INDEX);
	TextureIndex *index = &texture_cache[id];
	if(index->num_textures > 0) {
		int w = index->texture->w;
		int h = index->texture->h;
		if (w > cv_display_width->i_value) {
			w = (unsigned int) cv_display_width->i_value;
		}
		if (h > cv_display_height->i_value) {
			h = (unsigned int) cv_display_height->i_value;
		}

		plDrawTexturedRectangle(0, 0, w, h, index->texture);
#if 1
		for(unsigned int i = 0; i < index->num_textures; ++i) {
			plDrawRectangle(
					index->offsets[i].x,
					index->offsets[i].y,
					index->offsets[i].w,
					index->offsets[i].h,
					PL_COLOUR_RED
			);
		}
#endif
		char msg[256];
		snprintf(msg, sizeof(msg), "%u TEXTURES\n%dX%d\n%.2fMB (VRAM)",
				 index->num_textures,
				 index->texture->w,
				 index->texture->h,
				 plBytesToMegabytes(index->texture->size));
		Font_DrawBitmapString(g_fonts[FONT_SMALL], 10, 10, 0, 1.f, PL_COLOUR_WHITE, msg);
	} else {
		Font_DrawBitmapString(g_fonts[FONT_SMALL], 10, 10, 0, 1.f, PL_COLOUR_WHITE, "NO DATA CACHED!");
	}
}
#endif

#if 0 /* experimental palette changer thing... */
PLColour main_channel = PLColourRGB((uint8_t) (rand() % 255), (uint8_t) (rand() % 255), (uint8_t) (rand() % 255));
	plReplaceImageColour(&cache, PLColourRGB(90, 82, 8), main_channel);

	PLColour dark_channel = main_channel;
	if(dark_channel.r > 16) dark_channel.r -= 16;
	if(dark_channel.g > 25) dark_channel.g -= 25;
	if(dark_channel.b > 8)  dark_channel.b -= 8;
	plReplaceImageColour(&cache, PLColourRGB(74, 57, 0), dark_channel);

	PLColour light_channel = main_channel;
	if(light_channel.r < 206) light_channel.r += 49;
	if(light_channel.g < 197) light_channel.g += 58;
	if(light_channel.b < 214) light_channel.b += 41;
	plReplaceImageColour(&cache, PLColourRGB(139, 115, 49), light_channel);

	PLColour mid_channel = main_channel;
	/* this one still needs doing... */
	plReplaceImageColour(&cache, PLColourRGB(115, 98, 24), mid_channel);
#endif

bool Display_AppendVideoPreset( int width, int height ) {
	vidPresets.push_back( { width, height } );
	LogInfo( "Added %dx%d video preset", width, height );
	return true;
}

int Display_GetNumVideoPresets() {
	return vidPresets.size();
}

const VideoPreset *Display_GetVideoPreset( unsigned int idx ) {
	if ( idx >= vidPresets.size() ) {
		LogWarn( "Attempted to get an out of range video preset \'%d\', current count is %d", idx, vidPresets.size() );
		return nullptr;
	}

	return &vidPresets[ idx ];
}

/************************************************************/

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

	Camera *camera = Engine::Game()->GetCamera();
	camera->SetViewport( x, y, width, height );
}

int Display_GetViewportWidth( const PLViewport *viewport ) {
	return viewport->w;
}

int Display_GetViewportHeight( const PLViewport *viewport ) {
	return viewport->h;
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
		LogWarn( "failed to set display size to %dx%d!\n", cv_display_width->i_value, cv_display_height->i_value );
		LogInfo( "display set to %dx%d\n", w, h );

		// ... not sure if we'll force the engine to only render a specific display size ...
		if ( w < MIN_DISPLAY_WIDTH ) w = MIN_DISPLAY_WIDTH;
		if ( h < MIN_DISPLAY_HEIGHT ) h = MIN_DISPLAY_HEIGHT;

		plSetConsoleVariable( cv_display_width, pl_itoa( w, buf, 4, 10 ) );
		plSetConsoleVariable( cv_display_height, pl_itoa( h, buf, 4, 10 ) );
	} else {
		LogInfo( "display set to %dx%d\n", w, h );
	}

	Display_UpdateViewport( 0, 0, cv_display_width->i_value, cv_display_height->i_value );
}

void Display_Initialize() {
	// register debug vars
	cv_display_show_camerapos = plRegisterConsoleVariable( "display_show_camerapos", "0", pl_bool_var, nullptr, "" );
	cv_display_show_viewportinfo =
		plRegisterConsoleVariable( "display_show_viewportinfo", "0", pl_bool_var, nullptr, "" );

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
	System_DisplayWindow( false, MIN_DISPLAY_WIDTH, MIN_DISPLAY_HEIGHT );

	System_SetSwapInterval( cv_display_vsync->b_value ? 1 : 0 );
	System_SetWindowTitle( ENGINE_TITLE );

	Display_UpdateState();

	// platform library graphics subsystem can init now...
	plInitializeSubSystems( PL_SUBSYSTEM_GRAPHICS );
	plSetGraphicsMode( PL_GFX_MODE_OPENGL_CORE );

#if 0
	//Create the render textures
	game_target = plCreateFrameBuffer(640, 480, PL_BUFFER_COLOUR | PL_BUFFER_DEPTH);
	if (game_target == nullptr) {
	  Error("Failed to create game_target framebuffer, aborting!\n%s\n", plGetError());
	}

	frontend_target = plCreateFrameBuffer(640, 480, PL_BUFFER_COLOUR | PL_BUFFER_DEPTH);
	if (frontend_target == nullptr) {
	  Error("Failed to create frontend_target framebuffer, aborting!\n%s\n", plGetError());
	}
#endif

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

static void DrawDisplayInfo() {
	if ( !cv_display_show_viewportinfo->b_value ) {
		return;
	}

	char debug_display[128];
	int w, h;
	bool fs;
	System_GetWindowDrawableSize( &w, &h, &fs );
	sprintf( debug_display, "%d X %d %s", w, h, fs ? "FULLSCREEN" : "WINDOWED" );
	Font_DrawBitmapString( g_fonts[ FONT_GAME_CHARS ], 20,
						   Display_GetViewportHeight( &g_state.ui_camera->viewport ) - 32, 0, 1.f, PL_COLOUR_WHITE,
						   debug_display );
}

static void DrawFPSOverlay() {
	if ( !cv_debug_fps->b_value ) {
		return;
	}

	unsigned int fps, ms;
	Display_GetFramesCount( &fps, &ms );

	int w = Display_GetViewportWidth( &g_state.ui_camera->viewport );
	int h = Display_GetViewportHeight( &g_state.ui_camera->viewport );

	BitmapFont *font = g_fonts[ FONT_SMALL ];
	if ( font == nullptr ) {
		return;
	}

	char ms_count[32];
	sprintf( ms_count, "FPS %d/S (%d/MS)", fps, ms );
	unsigned int str_w = ( font->chars[ 0 ].w * 2 ) * strlen( ms_count );

	PLColour colour;
	if ( fps < 20 ) {
		colour = PL_COLOUR_RED;
	} else if ( fps < 30 ) {
		colour = PL_COLOUR_YELLOW;
	} else {
		colour = PL_COLOUR_GREEN;
	}

	PLVector2 position = PLVector2(
		static_cast< float >( w - str_w ),
		static_cast< float >( h - ( font->chars[ 0 ].h * 2 ) ) );
	PLVector2 size = PLVector2(
		static_cast< float >( str_w ),
		static_cast< float >( font->chars[ 0 ].h )
	);

	PLRectangle2D box = plCreateRectangle( position, size, PL_COLOUR_BLACK, PL_COLOUR_BLACK, PL_COLOUR_BLACK,
										   PL_COLOUR_BLACK );
	plDrawFilledRectangle( &box );

	Font_DrawBitmapString( font, position.x, position.y, 0, 1.f, colour, ms_count );
}

static void DrawCameraInfoOverlay() {
	if ( !cv_display_show_camerapos->b_value ) {
		return;
	}

	Camera *camera = Engine::Game()->GetCamera();
	if ( camera == nullptr ) {
		return;
	}

	PLVector3 position = camera->GetPosition();
	PLVector3 angles = camera->GetAngles();

	Font_DrawBitmapString( g_fonts[ FONT_CHARS2 ], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "CAMERA" );
	int y = 50;
	char cam_pos[32];
	snprintf( cam_pos, sizeof( cam_pos ), "POSITION : %s", plPrintVector3( &position, pl_float_var ) );
	Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y, 0, 1.f, PL_COLOUR_WHITE, cam_pos );
	snprintf( cam_pos, sizeof( cam_pos ), "ANGLES   : %s", plPrintVector3( &angles, pl_float_var ) );
	Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, cam_pos );
}

static void DrawDebugOverlay() {
	if ( cv_debug_mode->i_value <= 0 ) {
		return;
	}

	UI_DisplayDebugMenu(); /* aka imgui */

	if ( FrontEnd_GetState() == FE_MODE_INIT || FrontEnd_GetState() == FE_MODE_LOADING
		|| cv_debug_mode->i_value <= 0 ) {
		return;
	}

	DrawDisplayInfo();
	DrawCameraInfoOverlay();

#if 1
	Font_DrawBitmapString(g_fonts[FONT_CHARS2], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "DRAW STATS");
	unsigned int y = 50;
	char cam_pos[32];
	snprintf(cam_pos, sizeof(cam_pos), "CHUNKS DRAWN : %d", g_state.gfx.num_chunks_drawn);
	Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y, 0, 1.f, PL_COLOUR_WHITE, cam_pos);
	snprintf(cam_pos, sizeof(cam_pos), "ACTORS DRAWN : %d", g_state.gfx.num_actors_drawn);
	Font_DrawBitmapString(g_fonts[FONT_SMALL], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, cam_pos);
#endif

	if ( cv_debug_input->i_value > 0 ) {
		switch ( cv_debug_input->i_value ) {
			default: {
				Font_DrawBitmapString( g_fonts[ FONT_CHARS2 ], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "KEYBOARD STATE" );
				int x = 20, y = 50;
				for ( unsigned int i = 0; i < INPUT_MAX_KEYS; ++i ) {
					bool status = Input_GetKeyState( i );
					char key_state[64];
					snprintf( key_state, sizeof( key_state ), "%d (%s)", i, status ? "TRUE" : "FALSE" );
					Font_DrawBitmapString( g_fonts[ FONT_SMALL ], x, y, 0, 1.f, PL_COLOUR_WHITE, key_state );
					if ( y + 15 > Display_GetViewportHeight( &g_state.ui_camera->viewport ) - 50 ) {
						x += 90;
						y = 50;
					} else {
						y += 15;
					}
				}
			}
				break;

			case 2: {
				Font_DrawBitmapString( g_fonts[ FONT_CHARS2 ], 20, 24, 2, 1.f, PL_COLOUR_WHITE, "CONTROLLER STATE" );

				char button_state[64];
				snprintf( button_state, sizeof( button_state ), CHAR_PSX_CROSS " (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_CROSS ) ? "TRUE" : "FALSE" );
				unsigned int y = 50;
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), CHAR_PSX_TRIANGLE " (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_TRIANGLE ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), CHAR_PSX_CIRCLE " (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_CIRCLE ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), CHAR_PSX_SQUARE " (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_SQUARE ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), CHAR_PSX_L1 " (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_L1 ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), CHAR_PSX_L2 " (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_L2 ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), CHAR_PSX_R1 " (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_R1 ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), CHAR_PSX_R2 " (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_R2 ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), "START (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_START ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), "SELECT (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_SELECT ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), "UP (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_UP ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), "DOWN (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_DOWN ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), "LEFT (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_LEFT ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );

				snprintf( button_state, sizeof( button_state ), "RIGHT (%s)",
						  Input_GetButtonState( 0, INPUT_BUTTON_RIGHT ) ? "TRUE" : "FALSE" );
				Font_DrawBitmapString( g_fonts[ FONT_SMALL ], 20, y += 15, 0, 1.f, PL_COLOUR_WHITE, button_state );
			}
				break;
		}
		return;
	}
}

double cur_delta = 0;

static void Display_DrawMap() {
	Map *map = ohw::Engine::Game()->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	map->Draw();
}

void Display_DrawScene() {
	Camera *camera = Engine::Game()->GetCamera();
	if ( camera == nullptr ) {
		return;
	}

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
	Engine::Audio()->DrawSources();
	camera->DrawViewFrustum();
}

void Display_DrawInterface() {
	Shaders_SetProgramByName( "generic_textured" );

	plSetupCamera( g_state.ui_camera );
	plSetDepthBufferMode( PL_DEPTHBUFFER_DISABLE );
	FE_Draw();
}

void Display_DrawDebug() {
	Shaders_SetProgramByName( "generic_textured" );

	plSetupCamera( g_state.ui_camera );

	DrawDebugOverlay();
	DrawFPSOverlay();

	Console_Draw();
}

void Display_Draw( double delta ) {
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
	Display_DrawDebug();

	ImGuiImpl_Draw();

	Display_Flush();
}

void Display_Flush() {
	System_SwapDisplay();
	g_state.last_draw_ms = System_GetTicks() - g_state.draw_ticks;
}

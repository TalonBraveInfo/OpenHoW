// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Language.h"
#include "config.h"

using namespace ohw;

unsigned int ohw::LOG_LEVEL_DEFAULT = 0;
unsigned int ohw::LOG_LEVEL_WARNING = 0;
unsigned int ohw::LOG_LEVEL_ERROR = 0;
unsigned int ohw::LOG_LEVEL_DEBUG = 0;

/************************************************************/

#define check_args( num )                                                               \
	if ( argc < ( num ) ) {                                                             \
		Warning( "invalid number of arguments (%d < %d), ignoring!\n", argc, ( num ) ); \
		return;                                                                         \
	}

#if 0
static void FrontendModeCommand(unsigned int argc, char* argv[]) {
  check_args(2);

  int mode = atoi(argv[1]);
  if (mode < 0) {
	LogWarn("invalid frontend mode, \"%d\", specified!\n", mode);
	return;
  }

  FrontEnd_SetState((unsigned int) mode);
}
#endif

static void OpenCommand( unsigned int argc, char *argv[] ) {
	check_args( 2 );

	const char *fpath = argv[ 1 ];
	if ( fpath == nullptr ) {
		Warning( "invalid argument provided, ignoring!\n" );
		return;
	}

	enum {
		TYPE_UNKNOWN,
		TYPE_MODEL,
		TYPE_PARTICLE,
		TYPE_MAP,
		TYPE_SOUND,

		MAX_TYPES
	};
	unsigned int type = TYPE_UNKNOWN;

	/* now we just need to figure out what kind of file it is */
	const char *ext = PlGetFileExtension( fpath );
	if ( ext != nullptr ) {
		if ( pl_strncasecmp( ext, "vtx", 3 ) == 0 ||
		     pl_strncasecmp( ext, "fac", 3 ) == 0 ||
		     pl_strncasecmp( ext, "no2", 3 ) == 0 ) {
			type = TYPE_MODEL;
		} else if ( pl_strncasecmp( ext, "pmg", 3 ) == 0 ||
		            pl_strncasecmp( ext, "pog", 3 ) == 0 ||
		            pl_strncasecmp( ext, "map", 3 ) == 0 ) {
			type = TYPE_MAP;
		}
	}

	switch ( type ) {
		default:
			Warning( "unknown filetype, ignoring!\n" );
			break;

		case TYPE_MAP: {
			char map_name[ 32 ];
			snprintf( map_name, sizeof( map_name ), "%s", PlGetFileName( fpath ) - 4 );
			if ( *map_name == '\0' ) {
				Warning( "invalid map name, ignoring!\n" );
				return;
			}

			u_assert( 0 );
			//Map_Load(map_name, MAP_MODE_EDITOR);
			break;
		}
	}
}

/************************************************************/

// todo: kill all these global cvars - use variables instead w/ reference passed into RegisterConsoleVariable

PLConsoleVariable *cv_imgui = nullptr;
PLConsoleVariable *cv_debug_skeleton = nullptr;
PLConsoleVariable *cv_debug_bounds = nullptr;

PLConsoleVariable *cv_camera_fov = nullptr;
PLConsoleVariable *cv_camera_near = nullptr;
PLConsoleVariable *cv_camera_far = nullptr;

PLConsoleVariable *cv_display_width = nullptr;
PLConsoleVariable *cv_display_height = nullptr;
PLConsoleVariable *cv_display_fullscreen = nullptr;
PLConsoleVariable *cv_display_use_window_aspect = nullptr;
PLConsoleVariable *cv_display_ui_scale = nullptr;
PLConsoleVariable *cv_display_vsync = nullptr;

PLConsoleVariable *cv_graphics_cull = nullptr;
PLConsoleVariable *cv_GraphicsDrawTerrain = nullptr;
PLConsoleVariable *cv_graphics_draw_sprites = nullptr;
PLConsoleVariable *cv_graphics_draw_audio_sources = nullptr;
PLConsoleVariable *cv_graphics_texture_filter = nullptr;
PLConsoleVariable *cv_graphics_alpha_to_coverage = nullptr;
PLConsoleVariable *cv_graphics_debug_normals = nullptr;

PLConsoleVariable *cv_audio_volume = nullptr;
PLConsoleVariable *cv_audio_volume_music = nullptr;

void Console_Initialize() {
	cv_imgui = PlRegisterConsoleVariable( "imgui", "Enable/disable ImGui overlay.", "1", PL_VAR_BOOL, nullptr, nullptr, false );
	cv_debug_skeleton = PlRegisterConsoleVariable( "debug_skeleton", "display pig skeletons", "0", PL_VAR_BOOL, nullptr, nullptr, false );
	cv_debug_bounds = PlRegisterConsoleVariable( "debug_bounds", "Display bounding volumes of all objects.", "0", PL_VAR_BOOL, nullptr, nullptr, true );

	PlRegisterConsoleVariable( "game_language", "Set the language", "eng", PL_VAR_STRING, nullptr, &LanguageManager::SetLanguageCallback, true );

	PlRegisterConsoleVariable( "camera_mode", "0 = default, 1 = debug", "0", PL_VAR_I32, nullptr, nullptr, true );
	cv_camera_fov = PlRegisterConsoleVariable( "camera_fov", "field of view", "75", PL_VAR_F32, nullptr, nullptr, true );
	cv_camera_near = PlRegisterConsoleVariable( "camera_near", "", "0.1", PL_VAR_F32, nullptr, nullptr, true );
	cv_camera_far = PlRegisterConsoleVariable( "camera_far", "", "999999", PL_VAR_F32, nullptr, nullptr, true );

	cv_display_width = PlRegisterConsoleVariable( "display_width", "", "1024", PL_VAR_I32, nullptr, nullptr, true );
	cv_display_height = PlRegisterConsoleVariable( "display_height", "", "768", PL_VAR_I32, nullptr, nullptr, true );
	cv_display_fullscreen = PlRegisterConsoleVariable( "display_fullscreen", "", "true", PL_VAR_BOOL, nullptr, nullptr, true );
	cv_display_use_window_aspect = PlRegisterConsoleVariable( "display_use_window_aspect", "", "false", PL_VAR_BOOL, nullptr, nullptr, true );
	cv_display_ui_scale = PlRegisterConsoleVariable( "display_ui_scale", "0 = automatic scale", "0", PL_VAR_I32, nullptr, nullptr, true );
	cv_display_vsync = PlRegisterConsoleVariable(
	        "display_vsync", "Enable / Disable vertical sync", "false", PL_VAR_BOOL, nullptr, []( const PLConsoleVariable *variable ) {
		        Display *display = GetApp()->GetDisplay();
		        if ( display == nullptr ) {
			        return;
		        }

		        display->SetSwapInterval( variable->b_value ? 1 : 0 );
	        },
	        true );

	cv_graphics_cull = PlRegisterConsoleVariable( "graphics_cull", "Toggles culling of visible objects.", "true", PL_VAR_BOOL, nullptr, nullptr, true );
	PlRegisterConsoleVariable( "graphics_draw_terrain", "Toggles rendering of the terrain.", "true", PL_VAR_BOOL, nullptr, nullptr, true );
	cv_graphics_draw_sprites = PlRegisterConsoleVariable( "graphics_draw_sprites", "Toggles rendering of sprites.", "true", PL_VAR_BOOL, nullptr, nullptr, true );
	cv_graphics_draw_audio_sources = PlRegisterConsoleVariable( "graphics_draw_audio_sources", "toggles rendering of audio sources", "false", PL_VAR_BOOL, nullptr, nullptr, true );
	cv_graphics_texture_filter = PlRegisterConsoleVariable( "graphics_texture_filter", "Filter level/model textures?", "true", PL_VAR_BOOL, nullptr, nullptr, true );
	cv_graphics_alpha_to_coverage = PlRegisterConsoleVariable( "graphics_alpha_to_coverage", "Enable/disable alpha-to-coverage", "true", PL_VAR_BOOL, nullptr, nullptr, true );
	cv_graphics_debug_normals = PlRegisterConsoleVariable( "graphics_debug_normals", "Forces normals to be displayed", "false", PL_VAR_BOOL, nullptr, nullptr, true );

	cv_audio_volume = PlRegisterConsoleVariable( "audio_volume", "set global audio volume", "1", PL_VAR_F32, nullptr, nullptr, true );
	PlRegisterConsoleVariable( "audio_volume_sfx", "set sfx audio volume", "1", PL_VAR_F32, nullptr, nullptr, true );
	cv_audio_volume_music = PlRegisterConsoleVariable( "audio_volume_music", "Set the music audio volume", "1", PL_VAR_F32, nullptr, nullptr, true );
	PlRegisterConsoleVariable( "audio_mode", "0 = mono, 1 = stereo", "1", PL_VAR_I32, nullptr, nullptr, true );
	PlRegisterConsoleVariable( "audio_voices", "enable/disable pig voices", "true", PL_VAR_BOOL, nullptr, nullptr, true );

	PlRegisterConsoleCommand( "open", "Opens the specified file", 1, OpenCommand );

	PlRegisterConsoleCommand( "quit", "Closes the game", 0, []( unsigned int argc, char *argv[] ) {
		GetApp()->Shutdown();
	} );

	PlRegisterConsoleCommand( "loadConfig", "Loads the specified config", 1, []( unsigned int argc, char **argv ) {
		Config_Load( argv[ 1 ] );
	} );
	PlRegisterConsoleCommand( "saveConfig", "Save current config", 0, []( unsigned int argc, char **argv ) {
		const char *name = Config_GetUserConfigPath();
		if ( argc > 1 && argv[ 1 ] != nullptr ) {
			name = argv[ 1 ];
		}

		Config_Save( name );
	} );

	PlRegisterConsoleCommand( "disconnect", "Disconnects and unloads current map", 0, []( unsigned int argc, char *argv[] ) { GetApp()->gameManager->EndMode(); } );
	//PlRegisterConsoleCommand( "clear", ClearConsoleOutputBuffer, "Clears the console output buffer" );
	//PlRegisterConsoleCommand( "cls", ClearConsoleOutputBuffer, "Clears the console output buffer" );

	//ClearConsoleOutputBuffer( 0, nullptr );
	//plSetConsoleOutputCallback( ConsoleBufferUpdate );
}

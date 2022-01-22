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

#define check_args( num ) if(argc < (num)) { Warning("invalid number of arguments (%d < %d), ignoring!\n", argc, (num)); return; }

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

static void QuitCommand( unsigned int argc, char *argv[] ) {
	u_unused( argc );
	u_unused( argv );
	GetApp()->Shutdown();
}

static void DisconnectCommand( unsigned int argc, char *argv[] ) {
	u_unused( argc );
	u_unused( argv );
	GetApp()->gameManager->EndMode();
}

static void LoadConfigCommand( unsigned int argc, char **argv ) {
	check_args( 2 );
	Config_Load( argv[ 1 ] );
}

static void SaveConfigCommand( unsigned int argc, char **argv ) {
	const char *name = Config_GetUserConfigPath();
	if ( argc > 1 && argv[ 1 ] != nullptr ) {
		name = argv[ 1 ];
	}

	Config_Save( name );
}

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
			char map_name[32];
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

static void GraphicsVsyncCallback( const PLConsoleVariable *var ) {
	Display *display = GetApp()->GetDisplay();
	if ( display == nullptr ) {
		return;
	}

	display->SetSwapInterval( var->b_value ? 1 : 0 );
}

/************************************************************/

PLConsoleVariable *cv_debug_mode = nullptr;
PLConsoleVariable *cv_debug_skeleton = nullptr;
PLConsoleVariable *cv_debug_bounds = nullptr;

PLConsoleVariable *cv_game_language = nullptr;

PLConsoleVariable *cv_camera_mode = nullptr;
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
PLConsoleVariable *cv_audio_volume_sfx = nullptr;
PLConsoleVariable *cv_audio_volume_music = nullptr;
PLConsoleVariable *cv_audio_voices = nullptr;
PLConsoleVariable *cv_audio_mode = nullptr;

void Console_Initialize( void ) {
#define rvar( var, arc, ... ) \
    { \
        const char *str_##var = PL_STRINGIFY(var); \
        (var) = PlRegisterConsoleVariable(&str_##var[3], __VA_ARGS__); \
        (var)->archive = (arc); \
    }

	rvar( cv_debug_skeleton, false, "0", pl_bool_var, nullptr, "display pig skeletons" );
	rvar( cv_debug_bounds, false, "0", pl_bool_var, nullptr, "Display bounding volumes of all objects." );

	rvar( cv_game_language, true, "eng", pl_string_var, &LanguageManager::SetLanguageCallback, "Set the language" );

	rvar( cv_camera_mode, false, "0", pl_int_var, nullptr, "0 = default, 1 = debug" );
	rvar( cv_camera_fov, true, "75", pl_float_var, nullptr, "field of view" );
	rvar( cv_camera_near, false, "0.1", pl_float_var, nullptr, "" );
	rvar( cv_camera_far, false, "999999", pl_float_var, nullptr, "" );

	rvar( cv_display_width, true, "1024", pl_int_var, nullptr, "" );
	rvar( cv_display_height, true, "768", pl_int_var, nullptr, "" );
	rvar( cv_display_fullscreen, true, "true", pl_bool_var, nullptr, "" );
	rvar( cv_display_use_window_aspect, false, "false", pl_bool_var, nullptr, "" );
	rvar( cv_display_ui_scale, true, "0", pl_int_var, nullptr, "0 = automatic scale" );
	rvar( cv_display_vsync, true, "false", pl_bool_var, GraphicsVsyncCallback, "Enable / Disable vertical sync" );

	rvar( cv_graphics_cull, false, "true", pl_bool_var, nullptr, "Toggles culling of visible objects." );
	rvar( cv_GraphicsDrawTerrain, false, "true", pl_bool_var, nullptr, "Toggles rendering of the terrain." );
	rvar( cv_graphics_draw_sprites, false, "true", pl_bool_var, nullptr, "Toggles rendering of sprites." );
	rvar( cv_graphics_draw_audio_sources, false, "false", pl_bool_var, nullptr, "toggles rendering of audio sources" );
	rvar( cv_graphics_texture_filter, true, "true", pl_bool_var, nullptr, "Filter level/model textures?" );
	rvar( cv_graphics_alpha_to_coverage, true, "true", pl_bool_var, nullptr, "Enable/disable alpha-to-coverage" );
	rvar( cv_graphics_debug_normals, false, "false", pl_bool_var, nullptr, "Forces normals to be displayed" );

	rvar( cv_audio_volume, true, "1", pl_float_var, nullptr, "set global audio volume" );
	rvar( cv_audio_volume_sfx, true, "1", pl_float_var, nullptr, "set sfx audio volume" );
	rvar( cv_audio_volume_music, true, "1", pl_float_var, nullptr, "Set the music audio volume" );
	rvar( cv_audio_mode, true, "1", pl_int_var, nullptr, "0 = mono, 1 = stereo" );
	rvar( cv_audio_voices, true, "true", pl_bool_var, nullptr, "enable/disable pig voices" );

	PlRegisterConsoleCommand( "open", OpenCommand, "Opens the specified file" );
	PlRegisterConsoleCommand( "exit", QuitCommand, "Closes the game" );
	PlRegisterConsoleCommand( "quit", QuitCommand, "Closes the game" );
	PlRegisterConsoleCommand( "loadConfig", LoadConfigCommand, "Loads the specified config" );
	PlRegisterConsoleCommand( "saveConfig", SaveConfigCommand, "Save current config" );
	PlRegisterConsoleCommand( "disconnect", DisconnectCommand, "Disconnects and unloads current map" );
	//PlRegisterConsoleCommand( "clear", ClearConsoleOutputBuffer, "Clears the console output buffer" );
	//PlRegisterConsoleCommand( "cls", ClearConsoleOutputBuffer, "Clears the console output buffer" );

	//ClearConsoleOutputBuffer( 0, nullptr );
	//plSetConsoleOutputCallback( ConsoleBufferUpdate );
}

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

#include <PL/platform_graphics_camera.h>
#include <PL/platform_filesystem.h>

#include "engine.h"
#include "input.h"
#include "language.h"
#include "particle.h"
#include "frontend.h"
#include "graphics/display.h"
#include "graphics/font.h"
#include "config.h"

using namespace openhow;

/************************************************************/

#define check_args(num) if(argc < (num)) { LogWarn("invalid number of arguments (%d < %d), ignoring!\n", argc, (num)); return; }

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

static void UpdateDisplayCommand(unsigned int argc, char* argv[]) {
  Display_UpdateState();
}

static void QuitCommand(unsigned int argc, char* argv[]) {
  System_Shutdown();
}

static void DisconnectCommand(unsigned int argc, char* argv[]) {
  Engine::Game()->EndMode();
}

static void LoadConfigCommand(unsigned int argc, char** argv) {
  check_args(2);
  Config_Load(argv[1]);
}

static void SaveConfigCommand(unsigned int argc, char** argv) {
  const char* name = Config_GetUserConfigPath();
  if (argc > 1 && argv[1] != nullptr) {
    name = argv[1];
  }

  Config_Save(name);
}

static void OpenCommand(unsigned int argc, char* argv[]) {
  check_args(2);

  const char* fpath = argv[1];
  if (plIsEmptyString(fpath)) {
    LogWarn("invalid argument provided, ignoring!\n");
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
  const char* ext = plGetFileExtension(fpath);
  if (!plIsEmptyString(ext)) {
    if (pl_strncasecmp(ext, "vtx", 3) == 0 ||
        pl_strncasecmp(ext, "fac", 3) == 0 ||
        pl_strncasecmp(ext, "no2", 3) == 0) {
      type = TYPE_MODEL;
    } else if (pl_strncasecmp(ext, "pmg", 3) == 0 ||
        pl_strncasecmp(ext, "pog", 3) == 0 ||
        pl_strncasecmp(ext, "map", 3) == 0) {
      type = TYPE_MAP;
    } else if (pl_strncasecmp(ext, PPS_EXTENSION, 3) == 0) {
      type = TYPE_PARTICLE;
    }
  }

  switch (type) {
    default:
      LogWarn("unknown filetype, ignoring!\n");
      break;

    case TYPE_MAP: {
      char map_name[32];
      snprintf(map_name, sizeof(map_name), "%s", plGetFileName(fpath) - 4);
      if (plIsEmptyString(map_name)) {
        LogWarn("invalid map name, ignoring!\n");
        return;
      }

      u_assert(0);
      //Map_Load(map_name, MAP_MODE_EDITOR);
      break;
    }
  }
}

static void DebugModeCallback(const PLConsoleVariable* variable) {
  plSetupLogLevel(LOG_LEVEL_DEBUG, "debug", PLColour(0, 255, 255, 255), variable->b_value);
}

static void GraphicsVsyncCallback(const PLConsoleVariable* var) {
  System_SetSwapInterval(var->b_value ? 1 : 0);
}

/************************************************************/

#define MAX_INPUT_BUFFER_SIZE 256
#define MAX_OUTPUT_BUFFER_SIZE  4096

struct {
  char out_buffer[MAX_OUTPUT_BUFFER_SIZE];
  unsigned out_buffer_pos;

  char in_buffer[MAX_INPUT_BUFFER_SIZE];
  unsigned int in_buffer_pos;
} console_state;

static bool console_enabled = false;

PLConsoleVariable* cv_debug_mode = nullptr;
PLConsoleVariable* cv_debug_fps = nullptr;
PLConsoleVariable* cv_debug_skeleton = nullptr;
PLConsoleVariable* cv_debug_input = nullptr;
PLConsoleVariable* cv_debug_cache = nullptr;
PLConsoleVariable* cv_debug_shaders = nullptr;

PLConsoleVariable* cv_game_language = nullptr;

PLConsoleVariable* cv_camera_mode = nullptr;
PLConsoleVariable* cv_camera_fov = nullptr;
PLConsoleVariable* cv_camera_near = nullptr;
PLConsoleVariable* cv_camera_far = nullptr;

PLConsoleVariable* cv_display_texture_cache = nullptr;
PLConsoleVariable* cv_display_width = nullptr;
PLConsoleVariable* cv_display_height = nullptr;
PLConsoleVariable* cv_display_fullscreen = nullptr;
PLConsoleVariable* cv_display_use_window_aspect = nullptr;
PLConsoleVariable* cv_display_ui_scale = nullptr;
PLConsoleVariable* cv_display_vsync = nullptr;

PLConsoleVariable* cv_graphics_cull = nullptr;
PLConsoleVariable* cv_graphics_draw_world = nullptr;
PLConsoleVariable* cv_graphics_draw_sprites = nullptr;
PLConsoleVariable* cv_graphics_draw_audio_sources = nullptr;
PLConsoleVariable* cv_graphics_texture_filter = nullptr;
PLConsoleVariable* cv_graphics_alpha_to_coverage = nullptr;
PLConsoleVariable* cv_graphics_debug_normals = nullptr;

PLConsoleVariable* cv_audio_volume = nullptr;
PLConsoleVariable* cv_audio_volume_sfx = nullptr;
PLConsoleVariable* cv_audio_volume_music = nullptr;
PLConsoleVariable* cv_audio_voices = nullptr;
PLConsoleVariable* cv_audio_mode = nullptr;

static void ConsoleBufferUpdate(int level, const char* msg) {
  size_t len = strlen(msg);
  u_assert(len < MAX_OUTPUT_BUFFER_SIZE);
  if (len + console_state.out_buffer_pos > MAX_OUTPUT_BUFFER_SIZE) {
    unsigned int cpy_pos = 0;
    while (len + (console_state.out_buffer_pos - cpy_pos) > MAX_OUTPUT_BUFFER_SIZE
        || console_state.out_buffer[cpy_pos] != '\n') {
      ++cpy_pos;
    }

    memmove(console_state.out_buffer, console_state.out_buffer + cpy_pos, console_state.out_buffer_pos - cpy_pos);
    console_state.out_buffer_pos -= cpy_pos;
  }

  strncpy(console_state.out_buffer + console_state.out_buffer_pos, msg, len);
  console_state.out_buffer_pos += len;
}

static void ClearConsoleOutputBuffer(unsigned int argc, char** argv) {
  u_unused(argc);
  u_unused(argv);
  console_state.out_buffer_pos = 0;
}

void Console_Initialize(void) {
#define rvar(var, arc, ...) \
    { \
        const char *str_##var = plStringify(var); \
        (var) = plRegisterConsoleVariable(&str_##var[3], __VA_ARGS__); \
        (var)->archive = (arc); \
    }

  rvar(cv_debug_mode, false, "1", pl_int_var, DebugModeCallback, "global debug level");
  rvar(cv_debug_fps, false, "0", pl_bool_var, nullptr, "Display the framerate count while in-game");
  rvar(cv_debug_skeleton, false, "0", pl_bool_var, nullptr, "display pig skeletons");
  rvar(cv_debug_input, false, "0", pl_int_var, nullptr,
       "changing this cycles between different modes of debugging input\n"
       "1: keyboard states\n2: controller states"
  );
  rvar(cv_debug_cache, false, "0", pl_bool_var, nullptr, "display memory and other info");
  rvar(cv_debug_shaders, false, "-1", pl_int_var, nullptr, "Forces specified GLSL shader on all draw calls.");

  rvar(cv_game_language, true, "eng", pl_string_var, &LanguageManager::SetLanguageCallback, "Set the language");

  rvar(cv_camera_mode, false, "0", pl_int_var, nullptr, "0 = default, 1 = debug");
  rvar(cv_camera_fov, true, "75", pl_float_var, nullptr, "field of view");
  rvar(cv_camera_near, false, "0.1", pl_float_var, nullptr, "");
  rvar(cv_camera_far, false, "999999", pl_float_var, nullptr, "");

	rvar( cv_display_texture_cache, false, "-1", pl_int_var, nullptr, "" );
	rvar( cv_display_width, true, "1024", pl_int_var, nullptr, "" );
	rvar( cv_display_height, true, "768", pl_int_var, nullptr, "" );
	rvar( cv_display_fullscreen, true, "false", pl_bool_var, nullptr, "" );
	rvar( cv_display_use_window_aspect, false, "false", pl_bool_var, nullptr, "" );
	rvar( cv_display_ui_scale, true, "1", pl_int_var, nullptr, "0 = automatic scale" );
	rvar( cv_display_vsync, true, "false", pl_bool_var, GraphicsVsyncCallback, "Enable / Disable vertical sync" );

	rvar( cv_graphics_cull, false, "false", pl_bool_var, nullptr, "toggles culling of visible objects" );
	rvar( cv_graphics_draw_world, false, "true", pl_bool_var, nullptr, "toggles rendering of world" );
	rvar( cv_graphics_draw_sprites, false, "true", pl_bool_var, nullptr, "Toggles rendering of sprites." );
	rvar( cv_graphics_draw_audio_sources, false, "false", pl_bool_var, nullptr, "toggles rendering of audio sources" );
	rvar( cv_graphics_texture_filter, true, "false", pl_bool_var, nullptr, "Filter level/model textures?" );
	rvar( cv_graphics_alpha_to_coverage, true, "false", pl_bool_var, nullptr, "Enable/disable alpha-to-coverage" );
	rvar( cv_graphics_debug_normals, false, "false", pl_bool_var, nullptr, "Forces normals to be displayed" );

	rvar( cv_audio_volume, true, "1", pl_float_var, nullptr, "set global audio volume" );
	rvar( cv_audio_volume_sfx, true, "1", pl_float_var, nullptr, "set sfx audio volume" );
	rvar( cv_audio_volume_music, true, "1", pl_float_var, nullptr, "Set the music audio volume" );
	rvar( cv_audio_mode, true, "1", pl_int_var, nullptr, "0 = mono, 1 = stereo" );
	rvar( cv_audio_voices, true, "true", pl_bool_var, nullptr, "enable/disable pig voices" );

  plRegisterConsoleCommand("open", OpenCommand, "Opens the specified file");
  plRegisterConsoleCommand("exit", QuitCommand, "Closes the game");
  plRegisterConsoleCommand("quit", QuitCommand, "Closes the game");
  plRegisterConsoleCommand("loadConfig", LoadConfigCommand, "Loads the specified config");
  plRegisterConsoleCommand("saveConfig", SaveConfigCommand, "Save current config");
  plRegisterConsoleCommand("disconnect", DisconnectCommand, "Disconnects and unloads current map");
  plRegisterConsoleCommand("displayUpdate", UpdateDisplayCommand, "Updates the display to match current settings");
  //plRegisterConsoleCommand("femode", FrontendModeCommand, "Forcefully change the current mode for the frontend");
  plRegisterConsoleCommand("clear", ClearConsoleOutputBuffer, "Clears the console output buffer");
  plRegisterConsoleCommand("cls", ClearConsoleOutputBuffer, "Clears the console output buffer");

  ClearConsoleOutputBuffer(0, nullptr);
  plSetConsoleOutputCallback(ConsoleBufferUpdate);
}

static void DrawInputPane() {
  plSetTexture(nullptr, 0);
  plSetBlendMode(PL_BLEND_DEFAULT);

  BitmapFont* font = g_fonts[FONT_CHARS2];
  unsigned int scr_w = Display_GetViewportWidth(&g_state.ui_camera->viewport);
  unsigned int scr_h = Display_GetViewportHeight(&g_state.ui_camera->viewport);
  plDrawFilledRectangle(plCreateRectangle(
      PLVector2(0, scr_h - font->chars[0].h),
      PLVector2(scr_w, font->chars[0].h),
      PLColour(0, 0, 0, 255),
      PLColour(0, 0, 0, 0),
      PLColour(0, 0, 0, 255),
      PLColour(0, 0, 0, 0)
  ));

  plSetBlendMode(PL_BLEND_ADDITIVE);

  unsigned int x = 20;
  unsigned int y = scr_h - font->chars[0].h;

  char anim[] = {'I', '/', '-', '\\'};
  static unsigned int frame = 0;
  static double delay = 0;
  if (delay < g_state.sim_ticks) {
    if (++frame >= plArrayElements(anim)) {
      frame = 0;
    }
    delay = g_state.sim_ticks + 2;
  }

  Font_DrawBitmapCharacter(font, x, y, 1.f, PL_COLOUR_GREEN, anim[frame]);

  if (console_state.in_buffer_pos > 0) {
    char msg_buf[MAX_INPUT_BUFFER_SIZE];
    strncpy(msg_buf, console_state.in_buffer, sizeof(msg_buf));
    unsigned int w = font->chars[0].w;
    Font_DrawBitmapString(font, x + w + 10, y, 1, 1.f, PL_COLOUR_GREEN, pl_strtoupper(msg_buf));
  }

  /* DrawBitmapString disables blend - no need to call again here */

  plSetBlendMode(PL_BLEND_DEFAULT);
}

static void DrawOutputPane() {
  //unsigned int scr_w = Display_GetViewportWidth(&g_state.ui_camera->viewport);
  //unsigned int scr_h = Display_GetViewportHeight(&g_state.ui_camera->viewport);

  BitmapFont* font = g_fonts[FONT_CHARS2];
  for (size_t i = 0, cur_line = 0; i < console_state.out_buffer_pos;) {
    if (console_state.out_buffer[i] == '\n') {
      ++i;
      continue;
    }

    char* line_end = static_cast<char*>(memchr(console_state.out_buffer + i, '\n', console_state.out_buffer_pos - i));
    if (line_end == nullptr) {
      line_end = console_state.out_buffer + console_state.out_buffer_pos;
    }

    size_t line_len = line_end - (console_state.out_buffer + i);

    char line[512];
    strncpy(line, console_state.out_buffer + i, line_len);
    line[line_len] = '\0';

    int y = font->chars[0].h * cur_line;
    Font_DrawBitmapString(font, font->chars[0].w, y, 1, 1.0f, PL_COLOUR_GREEN, pl_strtoupper(line));

    cur_line++;
    i += line_len;
  }
}

void Console_Draw(void) {
  if (FrontEnd_GetState() == FE_MODE_INIT || FrontEnd_GetState() == FE_MODE_LOADING || !console_enabled) {
    return;
  }

  DrawInputPane();
  DrawOutputPane();
}

static void ResetInputBuffer() {
  memset(console_state.in_buffer, 0, sizeof(console_state.in_buffer));
  console_state.in_buffer_pos = 0;
}

static void ConsoleTextInputCallback(const char* c) {
  if (console_state.in_buffer_pos > MAX_INPUT_BUFFER_SIZE) {
    LogWarn("Hit console buffer limit!\n");
    return;
  }

  console_state.in_buffer[console_state.in_buffer_pos++] = *c;
}

static void ConsoleInputCallback(int key, bool pressed) {
  if (!pressed) {
    return;
  }

  if (key == '\r' && console_state.in_buffer[0] != '\0') {
    plParseConsoleString(console_state.in_buffer);
    ResetInputBuffer();
    return;
  }

  if (key == '\b' && console_state.in_buffer_pos > 0) {
    console_state.in_buffer[--console_state.in_buffer_pos] = '\0';
    return;
  }
}

void Console_Toggle(void) {
  console_enabled = !console_enabled;
  if (console_enabled) {
    Input_SetTextFocusCallback(ConsoleTextInputCallback);
    Input_SetKeyboardFocusCallback(ConsoleInputCallback);
    ResetInputBuffer();
  } else {
    Input_SetTextFocusCallback(nullptr);
    Input_SetKeyboardFocusCallback(nullptr);
  }

  Input_ResetStates();
}

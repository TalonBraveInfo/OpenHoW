// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

namespace ohw {
	extern unsigned int LOG_LEVEL_DEFAULT;
	extern unsigned int LOG_LEVEL_WARNING;
	extern unsigned int LOG_LEVEL_ERROR;
	extern unsigned int LOG_LEVEL_DEBUG;
}

#define CMSGPrintWFunction( LEVEL, FORMAT, ... ) PlLogMessage((LEVEL), "(%s) " FORMAT, PL_FUNCTION, ## __VA_ARGS__)

#if !defined( NDEBUG )
#   define DebugMsg( ... ) CMSGPrintWFunction(ohw::LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#   define DebugMsg( ... )
#endif

#define Print( ... )    CMSGPrintWFunction(ohw::LOG_LEVEL_DEFAULT, __VA_ARGS__)
#define Warning( ... )  CMSGPrintWFunction(ohw::LOG_LEVEL_WARNING, __VA_ARGS__)
#define Error( ... ) \
        CMSGPrintWFunction(ohw::LOG_LEVEL_ERROR, __VA_ARGS__);                               \
        ohw::GetApp()->DisplayMessageBox( ohw::App::MBErrorLevel::ERROR_MSG, __VA_ARGS__ ); \
        ohw::GetApp()->Shutdown()

/************************************************************/

extern PLConsoleVariable *cv_imgui;
extern PLConsoleVariable *cv_debug_skeleton;
extern PLConsoleVariable *cv_debug_bounds;

extern PLConsoleVariable *cv_game_language;

extern PLConsoleVariable *cv_camera_mode;
extern PLConsoleVariable *cv_camera_fov;
extern PLConsoleVariable *cv_camera_near;
extern PLConsoleVariable *cv_camera_far;

extern PLConsoleVariable *cv_display_width;
extern PLConsoleVariable *cv_display_height;
extern PLConsoleVariable *cv_display_fullscreen;
extern PLConsoleVariable *cv_display_use_window_aspect;
extern PLConsoleVariable *cv_display_ui_scale;
extern PLConsoleVariable *cv_display_vsync;

extern PLConsoleVariable *cv_graphics_cull;
extern PLConsoleVariable *cv_GraphicsDrawTerrain;
extern PLConsoleVariable *cv_graphics_draw_sprites;
extern PLConsoleVariable *cv_graphics_draw_audio_sources;
extern PLConsoleVariable *cv_graphics_texture_filter;
extern PLConsoleVariable *cv_graphics_alpha_to_coverage;
extern PLConsoleVariable *cv_graphics_debug_normals;

extern PLConsoleVariable *cv_audio_volume;
extern PLConsoleVariable *cv_audio_volume_sfx;
extern PLConsoleVariable *cv_audio_volume_music;
extern PLConsoleVariable *cv_audio_voices;
extern PLConsoleVariable *cv_audio_mode;

/************************************************************/

void Console_Initialize();

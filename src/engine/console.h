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

#pragma once

PL_EXTERN_C

extern PLConsoleVariable *cv_debug_mode;
extern PLConsoleVariable *cv_debug_fps;
extern PLConsoleVariable *cv_debug_skeleton;
extern PLConsoleVariable *cv_debug_shaders;
extern PLConsoleVariable *cv_debug_bounds;

extern PLConsoleVariable *cv_game_language;

extern PLConsoleVariable *cv_camera_mode;
extern PLConsoleVariable *cv_camera_fov;
extern PLConsoleVariable *cv_camera_near;
extern PLConsoleVariable *cv_camera_far;

extern PLConsoleVariable *cv_display_texture_cache;
extern PLConsoleVariable *cv_display_width;
extern PLConsoleVariable *cv_display_height;
extern PLConsoleVariable *cv_display_fullscreen;
extern PLConsoleVariable *cv_display_use_window_aspect;
extern PLConsoleVariable *cv_display_ui_scale;
extern PLConsoleVariable *cv_display_vsync;

extern PLConsoleVariable *cv_graphics_cull;
extern PLConsoleVariable* cv_graphics_draw_world;
extern PLConsoleVariable* cv_graphics_draw_sprites;
extern PLConsoleVariable* cv_graphics_draw_audio_sources;
extern PLConsoleVariable *cv_graphics_texture_filter;
extern PLConsoleVariable *cv_graphics_alpha_to_coverage;
extern PLConsoleVariable *cv_graphics_debug_normals;

extern PLConsoleVariable *cv_audio_volume;
extern PLConsoleVariable *cv_audio_volume_sfx;
extern PLConsoleVariable *cv_audio_volume_music;
extern PLConsoleVariable *cv_audio_voices;
extern PLConsoleVariable *cv_audio_mode;

/************************************************************/

void Console_Initialize(void);
void Console_Toggle(void);
void Console_Draw(void);

PL_EXTERN_C_END

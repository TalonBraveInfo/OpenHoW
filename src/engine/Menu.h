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

/*
 * Debrief > Continue / Retry
 *
 * Main Menu
 *  One Player
 *      New Game
 *          Select Team
 *              Please Name Your Team
 *                  Team Setup
 *                      Play Training Mission?
 *                          Loading Screen
 *      Load Game
 */

enum MinimapIcon {
	MINIMAP_ICON_BOMB,	// Unused?
	MINIMAP_ICON_HEALTH,
	MINIMAP_ICON_PIG,
	MINIMAP_ICON_PICKUP,
	MINIMAP_ICON_PROP,	// Unused?

	MAX_MINIMAP_ICONS
};

enum {
  FE_MODE_INIT,       /* menu shown during initialization */
  FE_MODE_START,      /* start screen, e.g. press any key */
  FE_MODE_LOADING,

  FE_MODE_MAIN_MENU,

  /* Main Menu */
  FE_MODE_ONE_PLAYER,
  FE_MODE_MULTI_PLAYER,
  FE_MODE_OPTIONS,
  FE_MODE_CONTROLS,

  /* One Player */
  FE_MODE_NEW_GAME,
  FE_MODE_LOAD_GAME,

  /* Multi-player */
  /* todo */

  /* Options */
  /* todo */

  FE_MODE_SELECT_TEAM,

  FE_MODE_EDITOR, /* editor mode - either embedded or standalone */
  FE_MODE_VIDEO,  /* playing a video - pressing escape will skip */
  FE_MODE_GAME,   /* in-game menu... probably cut this down? */

  MAX_FE_MODES
};

enum {
	FONT_BIG,
	FONT_BIG_CHARS,
	FONT_CHARS2,
	FONT_CHARS3,
	FONT_GAME_CHARS,
	FONT_SMALL,

	NUM_FONTS
};

// these are for FONT_SMALL, and probably will be made
// redundant once we get our own icons in etc.
#define CHAR_PSX_TRIANGLE   "c"
#define CHAR_PSX_CIRCLE     "d"
#define CHAR_PSX_CROSS      "e"
#define CHAR_PSX_SQUARE     "f"
#define CHAR_PSX_L1         "g"
#define CHAR_PSX_L2         "h"
#define CHAR_PSX_R1         "i"
#define CHAR_PSX_R2         "j"

#include "BitmapFont.h"

extern ohw::BitmapFont *g_fonts[NUM_FONTS];

void Menu_Initialize();
void Menu_UpdateViewport( int x, int y, int width, int height );
void FE_Shutdown();
void FE_ProcessInput();
void FrontEnd_Tick();
void Menu_Draw();

void FE_SetLoadingBackground(const char *name);
void FE_SetLoadingDescription(const char *description);
void FE_SetLoadingProgress(uint8_t progress);
uint8_t FE_GetLoadingProgress();

unsigned int FrontEnd_GetState();
void FrontEnd_SetState(unsigned int state);
void FE_RestoreLastState();

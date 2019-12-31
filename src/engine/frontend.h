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

PL_EXTERN_C

void FE_Initialize(void);
void FE_Shutdown(void);
void FE_ProcessInput(void);
void FrontEnd_Tick(void);
void FE_Draw(void);

void FE_SetLoadingBackground(const char *name);
void FE_SetLoadingDescription(const char *description);
void FE_SetLoadingProgress(uint8_t progress);
uint8_t FE_GetLoadingProgress(void);

unsigned int FrontEnd_GetState(void);
void FrontEnd_SetState(unsigned int state);
void FE_RestoreLastState(void);

PL_EXTERN_C_END

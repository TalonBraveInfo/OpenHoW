/* OpenHoW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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

enum {
    FE_MODE_INIT,       /* menu shown during initialization */
    FE_MODE_START,      /* start screen, e.g. press any key */
    FE_MODE_LOADING,

    FE_MODE_MAIN_MENU,      /* 'One Player' and other options */
    FE_MODE_SELECT_TEAM,    /* Team selection */

    FE_MODE_GAME,     /* in-game menu... probably cut this down? */
};

void InitFrontend(void);
void ProcessFrontendInput(void);
void SimulateFrontend(void);
void DrawFrontend(void);

void SetLoadingScreen(const char *name);
void SetLoadingDescription(const char *description);
void SetLoadingProgress(unsigned int progress);
unsigned int GetLoadingProgress(void);

unsigned int GetFrontendState(void);
void SetFrontendState(unsigned int state);
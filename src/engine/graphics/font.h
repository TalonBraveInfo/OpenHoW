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

#include "BitmapFont.h"

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

extern ohw::BitmapFont *g_fonts[NUM_FONTS];

void Font_CacheDefaultFonts();
void Font_ClearCachedFonts();

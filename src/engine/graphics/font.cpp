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
#include "font.h"

ohw::BitmapFont *g_fonts[NUM_FONTS];

void Font_CacheDefaultFonts() {
	// Cache all of the default fonts

	struct FontIndex {
		const char *tab, *texture;
	} defaultFonts[NUM_FONTS] = {
			{ "frontend/text/big.tab",       "frontend/text/big.bmp" },
			{ "frontend/text/bigchars.tab",  "frontend/text/bigchars.bmp" },
			{ "frontend/text/chars2.tab",    "frontend/text/chars2l.bmp" },
			{ "frontend/text/chars3.tab",    "frontend/text/chars3.bmp" },
			{ "frontend/text/gamechars.tab", "frontend/text/gamechars.bmp" },
			{ "frontend/text/small.tab",     "frontend/text/small.bmp" },
	};

	for ( unsigned int i = 0; i < NUM_FONTS; ++i ) {
		g_fonts[ i ] = new ohw::BitmapFont();
		if ( !g_fonts[ i ]->Load( defaultFonts[ i ].tab, defaultFonts[ i ].texture ) ) {
			Error( "Failed to load default font!\n" );
		}
	}
}

void Font_ClearCachedFonts() {
	for ( auto &g_font : g_fonts ) {
		delete g_font;
	}
}

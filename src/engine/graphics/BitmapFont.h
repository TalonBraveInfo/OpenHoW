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

namespace ohw {
	class BitmapFont {
	public:
		BitmapFont();
		~BitmapFont();

		bool Load( const char *tabPath, const char *texturePath );

		void DrawCharacter( float x, float y, float scale, PLColour colour, char character );
		void DrawString( float x, float y, float spacing, float scale, PLColour colour, const char *msg );

		PL_INLINE unsigned int GetCharacterWidth( unsigned char index ) const {
			u_assert( index < numChars );
			if ( index >= numChars ) {
				return 0;
			}

			return charTable[ index ].w;
		}

		PL_INLINE unsigned int GetCharacterHeight( unsigned char index ) const {
			u_assert( index < numChars );
			if ( index >= numChars ) {
				return 0;
			}

			return charTable[ index ].h;
		}

		struct TableIndex {
			unsigned int    x, y;
			unsigned int    w, h;
			char            character;
			float           s, t;
		};

	protected:
	private:
		TableIndex *GetIndexForChar( unsigned char index );

		void AddCharacterToPass( float x, float y, float scale, PLColour colour, char character );

#define MAX_BITMAP_CHARS 256
		TableIndex charTable[ MAX_BITMAP_CHARS ];
		unsigned int numChars{ 0 };

		SharedTextureResourcePointer texture{ nullptr };

		PLMesh *renderMesh{ nullptr };
	};
}

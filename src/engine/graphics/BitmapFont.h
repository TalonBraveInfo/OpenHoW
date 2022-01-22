// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

namespace ohw {
	class BitmapFont {
	public:
		BitmapFont();
		~BitmapFont();

		bool Load( const char *tabPath, const char *texturePath );

		void DrawCharacter( float x, float y, float scale, PLColour colour, char character );
		void DrawString( float x, float y, float spacing, float scale, PLColour colour, const char *msg );

		inline unsigned int GetCharacterWidth( unsigned char index ) const {
			u_assert( index < numChars );
			if ( index >= numChars ) {
				return 0;
			}

			return charTable[ index ].w;
		}

		inline unsigned int GetCharacterHeight( unsigned char index ) const {
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

		PLGMesh *renderMesh{ nullptr };
	};
}

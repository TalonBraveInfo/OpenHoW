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

#include "App.h"
#include "BitmapFont.h"
#include "Display.h"

ohw::BitmapFont::BitmapFont() {
	renderMesh = plCreateMesh( PL_MESH_TRIANGLES, PL_DRAW_DYNAMIC, 512, 256 );
	if ( renderMesh == nullptr ) {
		Error( "failed to create font mesh, %s, aborting!\n", plGetError() );
	}
}

ohw::BitmapFont::~BitmapFont() {
	plDestroyMesh( renderMesh );
}

bool ohw::BitmapFont::Load( const char *tabPath, const char *texturePath ) {
	PLFile *filePtr = plOpenFile( tabPath, false );
	if ( filePtr == nullptr ) {
		Warning( "Failed to load tab \"%s\"!\nPL: %s\n", tabPath, plGetError() );
		return false;
	}

	// Don't know, don't care
	plFileSeek( filePtr, 16, PL_SEEK_CUR );

	struct {
		uint16_t x, y;
		uint16_t w, h;
	} tabIndices[ MAX_BITMAP_CHARS ];
	numChars = ( unsigned int ) plReadFile( filePtr, tabIndices, sizeof( tabIndices ) / MAX_BITMAP_CHARS, MAX_BITMAP_CHARS );

	// We're done with the tab file
	plCloseFile( filePtr );

	if( numChars == 0 ) {
		Warning( "No characters in tab \"%s\"!\n" );
		return false;
	}

	texture = GetApp()->resourceManager->LoadTexture( texturePath, TextureResource::FLAG_DISCARD | TextureResource::FLAG_NOMIPS );

	// Now setup the character table
	unsigned int originX = tabIndices[ 0 ].x;
	unsigned int originY = tabIndices[ 0 ].y;
	for ( unsigned int i = 0; i < numChars; ++i ) {
		charTable[ i ].w = tabIndices[ i ].w;
		charTable[ i ].h = tabIndices[ i ].h;
		charTable[ i ].x = tabIndices[ i ].x - originX;
		charTable[ i ].y = tabIndices[ i ].y - originY;
	}

	return true;
}

/**
 * Immediately draw the given character.
 */
void ohw::BitmapFont::DrawCharacter( float x, float y, float scale, PLColour colour, char character ) {
	PLShaderProgram *program = plGetCurrentShaderProgram();
	if ( program == nullptr ) {
		return;
	}

	if ( scale <= 0.0f ) {
		return;
	}

	plSetTexture( texture->GetInternalTexture(), 0 );

	plClearMesh( renderMesh );

	AddCharacterToPass( x, y, scale, colour, character );

	PLMatrix4 matrix;
	matrix.Identity();

	plSetShaderUniformValue( program, "pl_model", &matrix, false );

	plUploadMesh( renderMesh );
	plDrawMesh( renderMesh );
}

/**
 * Immediately draw the given string.
 */
void ohw::BitmapFont::DrawString( float x, float y, float spacing, float scale, PLColour colour, const char *msg ) {
	PLShaderProgram *program = plGetCurrentShaderProgram();
	if ( program == nullptr ) {
		Error( "Attempted to draw bitmap string without a bound shader program!\n" );
	}

	if ( scale <= 0.0f ) {
		return;
	}

	size_t msgLength = strlen( msg );
	if ( msgLength == 0 ) {
		return;
	}

	plSetTexture( texture->GetInternalTexture(), 0 );

	plClearMesh( renderMesh );

	plMatrixMode( PL_MODELVIEW_MATRIX );
	plPushMatrix();

	plLoadIdentityMatrix();

	float nX = x;
	float nY = y;
	for ( size_t i = 0; i < msgLength; ++i ) {
		AddCharacterToPass( nX, nY, scale, colour, msg[ i ] );

		if ( msg[ i ] >= 33 && msg[ i ] <= 122 ) {
			nX += ( float ) charTable[ msg[ i ] - 33 ].w + spacing;
		} else if ( msg[ i ] == '\n' ) {
			nY += ( float ) charTable[ 0 ].h;
			nX = x;
		} else {
			nX += 5;
		}
	}

	plSetShaderUniformValue( program, "pl_model", plGetMatrix( PL_MODELVIEW_MATRIX ), false );

	plUploadMesh( renderMesh );
	plDrawMesh( renderMesh );

	plPopMatrix();
}

/**
 * Returns an index in the character table for the given character.
 */
ohw::BitmapFont::TableIndex *ohw::BitmapFont::GetIndexForChar( unsigned char index ) {
	// Fonts are sorta weird here, and only support a specific scope...
	if ( index < 33 || index > 138 ) {
		return nullptr;
	}

	return &charTable[ index - 33 ];
}

/**
 * Add the character onto the current draw pass - so essentially batching it.
 */
void ohw::BitmapFont::AddCharacterToPass( float x, float y, float scale, PLColour colour, char character ) {
	TableIndex *bitmapChar = GetIndexForChar( character );
	if ( bitmapChar == nullptr ) {
		return;
	}

	// Ensure it's on screen
	int dW, dH;
	GetApp()->GetDisplaySize( &dW, &dH );
	if ( x > dW || y > dH || x + bitmapChar->w < 0 || y + bitmapChar->h < 0 ) {
		return;
	}

	float tw = ( float ) bitmapChar->w / ( float ) texture->GetWidth();
	float th = ( float ) bitmapChar->h / ( float ) texture->GetHeight();
	float tx = ( float ) bitmapChar->x / ( float ) texture->GetWidth();
	float ty = ( float ) bitmapChar->y / ( float ) texture->GetHeight();

	unsigned int vX = plAddMeshVertex( renderMesh, PLVector3( x, y, 0 ), PLVector3(), colour, PLVector2( tx, ty ) );
	unsigned int vY = plAddMeshVertex( renderMesh, PLVector3( x, y + ( ( float ) bitmapChar->h * scale ), 0 ), PLVector3(), colour, PLVector2( tx, ty + th ) );
	unsigned int vZ = plAddMeshVertex( renderMesh, PLVector3( x + ( ( float ) bitmapChar->w * scale ), y, 0 ), PLVector3(), colour, PLVector2( tx + tw, ty ) );
	unsigned int vW = plAddMeshVertex( renderMesh, PLVector3( x + ( ( float ) bitmapChar->w * scale ), y + ( ( float ) bitmapChar->h * scale ), 0 ), PLVector3(), colour, PLVector2( tx + tw, ty + th ) );

	plAddMeshTriangle( renderMesh, vX, vY, vZ );
	plAddMeshTriangle( renderMesh, vZ, vY, vW );
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "BitmapFont.h"
#include "Display.h"

ohw::BitmapFont::BitmapFont() {
	renderMesh = PlgCreateMesh( PLG_MESH_TRIANGLES, PLG_DRAW_DYNAMIC, 512, 256 );
	if ( renderMesh == nullptr ) {
		Error( "failed to create font mesh, %s, aborting!\n", PlGetError() );
	}
}

ohw::BitmapFont::~BitmapFont() {
	PlgDestroyMesh( renderMesh );
}

bool ohw::BitmapFont::Load( const char *tabPath, const char *texturePath ) {
	PLFile *filePtr = PlOpenFile( tabPath, false );
	if ( filePtr == nullptr ) {
		Warning( "Failed to load tab \"%s\"!\nPL: %s\n", tabPath, PlGetError() );
		return false;
	}

	// Don't know, don't care
	PlFileSeek( filePtr, 16, PL_SEEK_CUR );

	struct {
		uint16_t x, y;
		uint16_t w, h;
	} tabIndices[ MAX_BITMAP_CHARS ];
	numChars = ( unsigned int ) PlReadFile( filePtr, tabIndices, sizeof( tabIndices ) / MAX_BITMAP_CHARS, MAX_BITMAP_CHARS );

	// We're done with the tab file
	PlCloseFile( filePtr );

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
	PLGShaderProgram *program = PlgGetCurrentShaderProgram();
	if ( program == nullptr ) {
		return;
	}

	if ( scale <= 0.0f ) {
		return;
	}

	PlgSetTexture( texture->GetInternalTexture(), 0 );

	PlgClearMesh( renderMesh );

	AddCharacterToPass( x, y, scale, colour, character );

	PLMatrix4 matrix = PlMatrix4Identity();

	PlgSetShaderUniformValue( program, "pl_model", &matrix, false );

	PlgUploadMesh( renderMesh );
	PlgDrawMesh( renderMesh );
}

/**
 * Immediately draw the given string.
 */
void ohw::BitmapFont::DrawString( float x, float y, float spacing, float scale, PLColour colour, const char *msg ) {
	PLGShaderProgram *program = PlgGetCurrentShaderProgram();
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

	PlgSetTexture( texture->GetInternalTexture(), 0 );

	PlgClearMesh( renderMesh );

	PlMatrixMode( PL_MODELVIEW_MATRIX );
	PlPushMatrix();

	PlLoadIdentityMatrix();

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

	PlgSetShaderUniformValue( program, "pl_model", PlGetMatrix( PL_MODELVIEW_MATRIX ), false );

	PlgUploadMesh( renderMesh );
	PlgDrawMesh( renderMesh );

	PlPopMatrix();
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

	Display *display = GetApp()->GetDisplay();
	if ( display == nullptr ) {
		return;
	}

	// Ensure it's on screen
	int dW, dH;
	display->GetDisplaySize( &dW, &dH );
	if ( x > dW || y > dH || x + bitmapChar->w < 0 || y + bitmapChar->h < 0 ) {
		return;
	}

	float tw = ( float ) bitmapChar->w / ( float ) texture->GetWidth();
	float th = ( float ) bitmapChar->h / ( float ) texture->GetHeight();
	float tx = ( float ) bitmapChar->x / ( float ) texture->GetWidth();
	float ty = ( float ) bitmapChar->y / ( float ) texture->GetHeight();

	unsigned int vX = PlgAddMeshVertex( renderMesh, hei::Vector3( x, y, 0 ), PLVector3(), colour, { tx, ty } );
	unsigned int vY = PlgAddMeshVertex( renderMesh, hei::Vector3( x, y + ( ( float ) bitmapChar->h * scale ), 0 ), PLVector3(), colour, { tx, ty + th } );
	unsigned int vZ = PlgAddMeshVertex( renderMesh, hei::Vector3( x + ( ( float ) bitmapChar->w * scale ), y, 0 ), PLVector3(), colour, { tx + tw, ty } );
	unsigned int vW = PlgAddMeshVertex( renderMesh, hei::Vector3( x + ( ( float ) bitmapChar->w * scale ), y + ( ( float ) bitmapChar->h * scale ), 0 ), PLVector3(), colour, { tx + tw, ty + th } );

	PlgAddMeshTriangle( renderMesh, vX, vY, vZ );
	PlgAddMeshTriangle( renderMesh, vZ, vY, vW );
}

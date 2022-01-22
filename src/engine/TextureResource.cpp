// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"

// TODO: we should be able to query the platform library for this!!
const char *supportedTextureFormats[] = { "png", "tga", "bmp", "tim", nullptr };

ohw::TextureResource::TextureResource( const std::string &path, unsigned int flags, bool persist, bool abortOnFail ) : Resource( path, persist ) {
	PLGTextureFilter filterMode;
	if ( flags & FLAG_NOMIPS ) {
		if ( flags & FLAG_NEAREST ) {
			filterMode = PLG_TEXTURE_FILTER_NEAREST;
		} else {
			filterMode = PLG_TEXTURE_FILTER_LINEAR;
		}
	} else {
		if ( flags & FLAG_NEAREST ) {
			filterMode = PLG_TEXTURE_FILTER_MIPMAP_NEAREST;
		} else {
			filterMode = PLG_TEXTURE_FILTER_MIPMAP_LINEAR;
		}
	}

	const char *fileExtension = PlGetFileExtension( path.c_str() );
	if ( fileExtension[ 0 ] == '\0' ) {
		const char *newPath = u_find2( path.c_str(), supportedTextureFormats, abortOnFail );
		if ( newPath != nullptr ) {
			texturePtr = PlgLoadTextureFromImage( newPath, filterMode );
		}

		if ( texturePtr == nullptr ) {
			if ( abortOnFail ) {
				Error( "Failed to load texture, \"%s\"!\nPL: %s\n", path.c_str(), PlGetError() );
			}

			Warning( "Failed to load texture, \"%s\"!\nPL: %s\n", path.c_str(), PlGetError() );
			texturePtr = GetApp()->resourceManager->GetFallbackTexture();
		}

		return;
	}

	PLImage *image = PlLoadImage( path.c_str() );
	if ( image ) {
		// pixel format of TIM will be changed before uploading
		if ( pl_strncasecmp( fileExtension, "tim", 3 ) == 0 ) {
			PlConvertPixelFormat( image, PL_IMAGEFORMAT_RGBA8 );
		}

		// If discard is specified, we need to throw away the first colour
		if ( flags & FLAG_DISCARD ) {
			const PLColour *firstColour = ( PLColour * ) image->data[ 0 ];
			PlReplaceImageColour( image, *firstColour, { 0, 0, 0, 0 } );
		}

		texturePtr = PlgCreateTexture();
		if ( texturePtr != nullptr ) {
			texturePtr->filter = filterMode;
			if ( PlgUploadTextureImage( texturePtr, image ) ) {
				return;
			}
		}

		PlgDestroyTexture( texturePtr );
	}

	PlDestroyImage( image );

	if ( abortOnFail ) {
		Error( "Failed to load texture, \"%s\"!\nPL: %s\n", path.c_str(), PlGetError() );
	}

	Warning( "Failed to load texture, \"%s\"!\nPL: %s\n", path.c_str(), PlGetError() );

	texturePtr = GetApp()->resourceManager->GetFallbackTexture();
}

ohw::TextureResource::~TextureResource() {
	// Don't destroy the fallback!
	PLGTexture *placeholderTexture = GetApp()->resourceManager->GetFallbackTexture();
	if ( texturePtr == placeholderTexture ) {
		return;
	}

	PlgDestroyTexture( texturePtr );
}

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
#include "TextureResource.h"

// TODO: we should be able to query the platform library for this!!
const char *supportedTextureFormats[] = { "png", "tga", "bmp", "tim", nullptr };

ohw::TextureResource::TextureResource( const std::string &path, unsigned int flags, bool persist, bool abortOnFail ) : Resource( path, persist ) {
	PLTextureFilter filterMode;
	if ( flags & FLAG_NOMIPS ) {
		if ( flags & FLAG_NEAREST ) {
			filterMode = PL_TEXTURE_FILTER_NEAREST;
		} else {
			filterMode = PL_TEXTURE_FILTER_LINEAR;
		}
	} else {
		if ( flags & FLAG_NEAREST ) {
			filterMode = PL_TEXTURE_FILTER_MIPMAP_NEAREST;
		} else {
			filterMode = PL_TEXTURE_FILTER_MIPMAP_LINEAR;
		}
	}

	const char *fileExtension = plGetFileExtension( path.c_str() );
	if ( fileExtension[ 0 ] == '\0' ) {
		const char *newPath = u_find2( path.c_str(), supportedTextureFormats, abortOnFail );
		if ( newPath != nullptr ) {
			texturePtr = plLoadTextureFromImage( newPath, filterMode );
		}

		if ( texturePtr == nullptr ) {
			if ( abortOnFail ) {
				Error( "Failed to load texture, \"%s\"!\nPL: %s\n", path.c_str(), plGetError() );
			}

			LogWarn( "Failed to load texture, \"%s\"!\nPL: %s\n", path.c_str(), plGetError() );
			texturePtr = ohw::Engine::Resource()->GetFallbackTexture();
		}

		return;
	}

	PLImage *image = plLoadImage( path.c_str() );
	if ( image ) {
		// pixel format of TIM will be changed before uploading
		if ( pl_strncasecmp( fileExtension, "tim", 3 ) == 0 ) {
			plConvertPixelFormat( image, PL_IMAGEFORMAT_RGBA8 );
		}

		// If discard is specified, we need to throw away the first colour
		if ( flags & FLAG_DISCARD ) {
			const PLColour *firstColour = ( PLColour * ) image->data[ 0 ];
			plReplaceImageColour( image, *firstColour, PLColour( 0, 0, 0, 0 ) );
		}

		texturePtr = plCreateTexture();
		if ( texturePtr != nullptr ) {
			texturePtr->filter = filterMode;
			if ( plUploadTextureImage( texturePtr, image ) ) {
				return;
			}
		}

		plDestroyTexture( texturePtr );
	}

	plDestroyImage( image );

	if ( abortOnFail ) {
		Error( "Failed to load texture, \"%s\"!\nPL: %s\n", path.c_str(), plGetError() );
	}

	LogWarn( "Failed to load texture, \"%s\"!\nPL: %s\n", path.c_str(), plGetError() );

	texturePtr = ohw::Engine::Resource()->GetFallbackTexture();
}

ohw::TextureResource::~TextureResource() {
	// Don't destroy the fallback!
	PLTexture *placeholderTexture = Engine::Resource()->GetFallbackTexture();
	if ( texturePtr == placeholderTexture ) {
		return;
	}

	plDestroyTexture( texturePtr );
}

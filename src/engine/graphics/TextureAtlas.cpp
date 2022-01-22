// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Display.h"
#include "TextureAtlas.h"

ohw::TextureAtlas::TextureAtlas( int w, int h ) : width_( w ), height_( h ) {
	texture_ = ohw::GetApp()->resourceManager->GetFallbackTexture();
}

ohw::TextureAtlas::~TextureAtlas() {
	for ( auto &id: images_by_name_ ) {
		PLImage *image = id.second;
		PlDestroyImage( image );
		id.second = nullptr;
	}

	if ( texture_ != ohw::GetApp()->resourceManager->GetFallbackTexture() ) {
		// TODO: reintroduce once we have a wrapper around PLModel to hold this!
		//plDestroyTexture(texture_);
	}
}

bool ohw::TextureAtlas::AddImage( const std::string &path, bool absolute ) {
	const auto image = images_by_name_.find( path );
	if ( image != images_by_name_.end() ) {
		return true;
	}

	char full_path[PL_SYSTEM_MAX_PATH];
	if ( absolute ) {
		strncpy( full_path, path.c_str(), sizeof( full_path ) - 1 );
	} else {
		snprintf( full_path, sizeof( full_path ) - 1, "%s", u_find2( path.c_str(), supportedTextureFormats, false ) );
	}

	PLImage *img = PlLoadImage( full_path );
	if ( img == nullptr ) {
		return false;
	}

	PlConvertPixelFormat( img, PL_IMAGEFORMAT_RGBA8 );
	images_by_name_.emplace( path, img );
	images_by_height_.emplace( img->height, img );
	return true;
}

void ohw::TextureAtlas::AddImages( const std::vector< std::string > &textures ) {
	for ( const auto &path: textures ) {
		AddImage( path );
	}
}

void ohw::TextureAtlas::Finalize() {
	if ( images_by_height_.empty() ) {
		Warning( "Failed to finalize texture atlas, no textures loaded!\n" );
		return;
	}

	// Figure out how we'll organise the atlas
	unsigned int w = width_, h = height_;
	unsigned int max_h = 0;
	unsigned int cur_y = 0, cur_x = 0;
	for ( auto i = images_by_height_.rbegin(); i != images_by_height_.rend(); ++i ) {
		PLImage *image = i->second;
		if ( image->height > max_h ) {
			max_h = image->height;
		}

		if ( cur_x == 0 && image->width > w ) {
			w = image->width;
		} else if ( cur_x + image->width > w ) {
			cur_y += max_h;
			cur_x = max_h = 0;
		}

		if ( cur_y + image->height > h ) {
			h = cur_y + image->height;
		}

		u_assert( image->path[ 0 ] != '\0', "Invalid image name!" );
		const char *filename = PlGetFileName( image->path );
		const char *extension = PlGetFileExtension( image->path );
		std::string index_name = std::string( filename ).substr( 0, strlen( filename ) - ( strlen( extension ) + 1 ) );
		textures_.emplace( index_name, Index{
				.x = cur_x,
				.y = cur_y,
				.w = image->width,
				.h = image->height,
				.image = image
		} );

		cur_x += image->width;
	}

	// Ensure power of two for final atlas
	h = static_cast<unsigned int>(pow( 2, ceil( log( h ) / log( 2 ) ) ));
	w = static_cast<unsigned int>(pow( 2, ceil( log( w ) / log( 2 ) ) ));

	// Image pointers will be freed when we're done with textures list
	images_by_name_.clear();
	images_by_height_.clear();

	// Now create the atlas itself
	PLImage *cache = PlCreateImage( nullptr, w, h, PL_COLOURFORMAT_RGBA, PL_IMAGEFORMAT_RGBA8 );
	if ( cache == nullptr ) {
		Error( "Failed to generate image cache for texture atlas (%s)!\n", PlGetError() );
	}

	//plReplaceImageColour(cache, {0, 0, 0, 0}, {0, 0, 0, 255});

	for ( auto &tarr: textures_ ) {
		Index *texture = &tarr.second;
		uint8_t *pos = cache->data[ 0 ] + ( ( texture->y * cache->width ) + texture->x ) * 4;
		uint8_t *src = texture->image->data[ 0 ];
		for ( unsigned int y = 0; y < texture->h; ++y ) {
			memcpy( pos, src, ( texture->w * 4 ) );
			src += texture->w * 4;
			pos += cache->width * 4;
		}

		PlDestroyImage( texture->image );
		texture->image = nullptr;
	}

#ifdef _DEBUG
	static unsigned int gen_id = 0;
	if ( PlCreatePath( "./debug/generated/" ) ) {
		char buf[PL_SYSTEM_MAX_PATH];
		snprintf( buf, sizeof( buf ) - 1, "./debug/generated/%dx%d_%d.png",
		          cache->width, cache->height, gen_id++ );
		PlWriteImage( cache, buf );
	}
#endif

	if ( ( texture_ = PlgCreateTexture() ) == nullptr ) {
		Error( "Failed to generate atlas texture (%s)!\n", PlGetError() );
	}

	if ( cv_graphics_texture_filter->b_value ) {
		PlgSetTextureAnisotropy( texture_, 8 );
		texture_->filter = PLG_TEXTURE_FILTER_MIPMAP_LINEAR;
	} else {
		PlgSetTextureAnisotropy( texture_, 0 );
		texture_->filter = PLG_TEXTURE_FILTER_MIPMAP_NEAREST;
	}

	if ( !PlgUploadTextureImage( texture_, cache ) ) {
		Error( "Failed to upload texture atlas (%s)!\n", PlGetError() );
	}

	PlDestroyImage( cache );
}

bool ohw::TextureAtlas::GetTextureCoords( const std::string &name, float *x, float *y, float *w, float *h ) {
	auto index = textures_.find( name );
	if ( index == textures_.end() ) {
		*x = *y = 0;
		*w = *h = 1.0f;
		return false;
	}

	// HACK: work around texture leaking until we have a better solution!
	int shift = 0;
	if ( texture_->filter == PLG_TEXTURE_FILTER_MIPMAP_LINEAR ) {
		shift = 1;
	}

	*x = static_cast<float>(index->second.x + shift) / static_cast<float>(texture_->w);
	*y = static_cast<float>(index->second.y + shift) / static_cast<float>(texture_->h);
	*w = static_cast<float>(index->second.w - shift * 2) / static_cast<float>(texture_->w);
	*h = static_cast<float>(index->second.h - shift * 2) / static_cast<float>(texture_->h);
	return true;
}

std::pair< unsigned int, unsigned int > ohw::TextureAtlas::GetTextureSize( const std::string &name ) {
	auto index = textures_.find( name );
	if ( index == textures_.end() ) {
		return std::make_pair( texture_->w, texture_->h );
	}

	return std::make_pair( index->second.w, index->second.h );
}

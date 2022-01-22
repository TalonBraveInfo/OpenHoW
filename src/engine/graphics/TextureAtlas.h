// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

namespace ohw {
	class TextureAtlas {
	public:
		TextureAtlas( int w, int h );
		~TextureAtlas();

		bool GetTextureCoords( const std::string &name, float *x, float *y, float *w, float *h );
		std::pair< unsigned int, unsigned int > GetTextureSize( const std::string &name );

		bool AddImage( const std::string &path, bool absolute = false );
		void AddImages( const std::vector< std::string > &textures );

		void Finalize();

		PLGTexture *GetTexture() { return texture_; }

	protected:
	private:
		struct Index {
			unsigned int x, y, w, h;
			PLImage *image;
		};

		int width_{ 512 };
		int height_{ 8 };

		std::map< std::string, Index > textures_;
		std::map< std::string, PLImage * > images_by_name_;
		std::multimap< unsigned int, PLImage * > images_by_height_;

		PLGTexture *texture_{ nullptr };
	};
}

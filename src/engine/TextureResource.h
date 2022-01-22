// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include "Resource.h"

namespace ohw {
	class TextureResource : public Resource {
	public:
		IMPLEMENT_RESOURCE_CLASS( TextureResource )

		explicit TextureResource( const std::string &path, unsigned int flags = 0, bool persist = false, bool abortOnFail = false );
		~TextureResource();

		PLGTexture *GetInternalTexture() { return texturePtr; }

		inline unsigned int GetWidth() const { return texturePtr->w; }
		inline unsigned int GetHeight() const { return texturePtr->h; }

		inline size_t GetTextureSize() const { return texturePtr->size; }

		enum {
			FLAG_DISCARD = ( 1 << 0 ),   // Convert the background colour to alpha
			FLAG_NOMIPS = ( 1 << 1 ),   // Will not generate mipmaps
			FLAG_NEAREST = ( 1 << 2 ),   // Will use nearest filtering
		};

	private:
		PLGTexture *texturePtr{ nullptr };
	};

	using SharedTextureResourcePointer = SharedResourcePointer< TextureResource >;
}

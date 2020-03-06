/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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

namespace openhow {
class Engine;
}

class ResourceManager {
private:
	ResourceManager();
	~ResourceManager();

public:
	PLTexture* GetCachedTexture( const std::string& path );
	PLModel* GetCachedModel( const std::string& path );

	PLTexture* LoadTexture( const std::string& path,
							PLTextureFilter filter = PL_TEXTURE_FILTER_MIPMAP_NEAREST,
							bool persist = false, bool abort_on_fail = false );
	PLModel* LoadModel( const std::string& path, bool persist = false, bool abort_on_fail = false );

	PLTexture* GetFallbackTexture();
	PLModel* GetFallbackModel();

	void ClearTextures( bool force = false );
	void ClearModels( bool force = false );

	void ClearAll();

private:
	static void ListCachedResources( unsigned int argc, char** argv );
	static void ClearTexturesCommand( unsigned int argc, char** argv );
	static void ClearModelsCommand( unsigned int argc, char** argv );

	struct TextureHandle {
		TextureHandle( PLTexture* texture_ptr, bool persist ) {
			this->texture_ptr = texture_ptr;
			this->persist = persist;
		}

		PLTexture* texture_ptr{ nullptr };
		bool persist{ false };
	};
	std::map<std::string, TextureHandle> textures_;
	inline PLTexture* CacheTexture( const std::string& path, PLTexture* texture_ptr, bool persist = false ) {
		textures_.insert( std::pair<std::string, TextureHandle>( path, { texture_ptr, persist } ) );
		return texture_ptr;
	}

	struct ModelHandle {
		ModelHandle( PLModel* model_ptr, bool persist ) {
			this->model_ptr = model_ptr;
			this->persist = persist;
		}

		PLModel* model_ptr{ nullptr };
		bool persist{ false };
	};
	std::map<std::string, ModelHandle> models_;
	inline PLModel* CacheModel( const std::string& path, PLModel* model_ptr, bool persist = false ) {
		models_.insert( std::pair<std::string, ModelHandle>( path, { model_ptr, persist } ) );
		return model_ptr;
	}

	PLTexture* fallback_texture_{ nullptr };
	PLModel* fallback_model_{ nullptr };

	friend class openhow::Engine;
};

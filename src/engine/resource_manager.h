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

#include "ModelResource.h"
#include "TextureResource.h"

namespace ohw {
	class ResourceManager {
	private:
		ResourceManager();
		~ResourceManager();

	public:
		TextureResource *LoadTexture( const std::string &path, PLTextureFilter filter = PL_TEXTURE_FILTER_MIPMAP_NEAREST, bool persist = false, bool abortOnFail = false );
		ModelResource *LoadModel( const std::string &path, bool persist = false, bool abortOnFail = false );

		void ClearResource( const std::string &path, bool force = false );
		void ClearAllResources( bool force = false );

		PLTexture *GetFallbackTexture();
		PLModel *GetFallbackModel();

	private:
		static void ListCachedResources( unsigned int argc, char **argv );
		static void ClearAllResourcesCommand( unsigned int argc, char **argv );
		static void ClearResourceCommand( unsigned int argc, char **argv );

		PLTexture *fallbackTexture;
		PLModel *fallbackModel;

		Resource *GetCachedResource( const std::string &path );
		PL_INLINE Resource *CacheResource( const std::string &path, Resource *resourcePtr, bool persist = false ) {
			resourcesMap.insert( std::pair< std::string, Resource* >( path, resourcePtr ) );
			LogDebug( "Cached resource, \"%s\"\n", path.c_str() );
			return resourcePtr;
		}

		std::map< std::string, Resource* > resourcesMap;

		friend class Engine;
	};
}

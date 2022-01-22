// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include <map>

#include "ModelResource.h"
#include "TextureResource.h"

namespace ohw {
	class ResourceManager {
	private:
		ResourceManager();
		~ResourceManager();

	public:
		SharedTextureResourcePointer LoadTexture( const std::string &path, unsigned int flags = 0, bool persist = false, bool abortOnFail = false );
		SharedModelResourcePointer LoadModel( const std::string &path, bool persist = false, bool abortOnFail = false );

		void ClearResource( const std::string &path, bool force = false );
		void ClearAllResources( bool force = false );

		PLGTexture *GetFallbackTexture();
		PLMModel *GetFallbackModel();

	private:
		static void ListCachedResources( unsigned int argc, char **argv );
		static void ClearAllResourcesCommand( unsigned int argc, char **argv );
		static void ClearResourceCommand( unsigned int argc, char **argv );

		PLGTexture *fallbackTexture{ nullptr };
		PLMModel *fallbackModel{ nullptr };

		Resource *GetCachedResource( const std::string &path );
		inline Resource *CacheResource( const std::string &path, Resource *resourcePtr, bool persist = false ) {
			resourcesMap.insert( std::pair< std::string, Resource* >( path, resourcePtr ) );
			//DebugMsg( "Cached resource, \"%s\"\n", path.c_str() );
			return resourcePtr;
		}

		std::map< std::string, Resource* > resourcesMap;

		friend class App;
	};
}

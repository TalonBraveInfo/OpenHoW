// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "ResourceManager.h"
#include "ShaderManager.h"

ohw::ResourceManager::ResourceManager() {
	// Allow users to enable support for all package formats if desired (disabled by default for security reasons)
	if ( PlHasCommandLineArgument( "-rapf" ) ) {
		PlRegisterStandardPackageLoaders();
	}

	PlRegisterConsoleCommand( "ListCachedResources", "List all cached resources.", 0, &ResourceManager::ListCachedResources );
	PlRegisterConsoleCommand( "ClearAllResources", "Clears all cached resources.", 0, &ResourceManager::ClearAllResourcesCommand );
	PlRegisterConsoleCommand( "ClearResource", "Clears the specified resource.", 0, &ResourceManager::ClearResourceCommand );
}

ohw::ResourceManager::~ResourceManager() {
	ClearAllResources( true );
}

ohw::Resource *ohw::ResourceManager::GetCachedResource( const std::string &path ) {
	auto idx = resourcesMap.find( path );
	if ( idx != resourcesMap.end() ) {
		return idx->second;
	}

	return nullptr;
}

ohw::SharedTextureResourcePointer ohw::ResourceManager::LoadTexture( const std::string &path, unsigned int flags, bool persist, bool abortOnFail ) {
	TextureResource *texturePtr = static_cast< TextureResource * >( GetCachedResource( path ) );
	if ( texturePtr != nullptr ) {
		return texturePtr;
	}

	texturePtr = new TextureResource( path, flags, persist, abortOnFail );
	CacheResource( path, texturePtr );

	return texturePtr;
}

ohw::SharedModelResourcePointer ohw::ResourceManager::LoadModel( const std::string &path, bool persist, bool abortOnFail ) {
	ModelResource *modelPtr = static_cast< ModelResource * >( GetCachedResource( path ) );
	if ( modelPtr != nullptr ) {
		return modelPtr;
	}

	modelPtr = new ModelResource( path, persist, abortOnFail );
	CacheResource( path, modelPtr );

	return modelPtr;
}

PLGTexture *ohw::ResourceManager::GetFallbackTexture() {
	if ( fallbackTexture != nullptr ) {
		return fallbackTexture;
	}

	PLColour pixelBuffer[] = {
			{ 255, 255, 0,   255 },
			{ 0,   255, 255, 255 },
			{ 0,   255, 255, 255 },
			{ 255, 255, 0,   255 } };
	PLImage *image = PlCreateImage( ( uint8_t * ) pixelBuffer, 2, 2, PL_COLOURFORMAT_RGBA, PL_IMAGEFORMAT_RGBA8 );
	if ( image == nullptr ) {
		Error( "Failed to generate default texture (%s)!\n", PlGetError() );
	}

	fallbackTexture = PlgCreateTexture();
	fallbackTexture->flags &= PLG_TEXTURE_FLAG_NOMIPS;
	if ( !PlgUploadTextureImage( fallbackTexture, image ) ) {
		Error( "Failed to upload default texture (%s)!\n", PlGetError() );
	}

	PlDestroyImage( image );

	return fallbackTexture;
}

PLMModel *ohw::ResourceManager::GetFallbackModel() {
	if ( fallbackModel != nullptr ) {
		return fallbackModel;
	}

	PLGMesh *mesh = PlgCreateMesh( PLG_MESH_LINES, PLG_DRAW_DYNAMIC, 0, 6 );
	PlgSetMeshVertexPosition( mesh, 0, { 0, 20, 0 } );
	PlgSetMeshVertexPosition( mesh, 1, { 0, -20, 0 } );
	PlgSetMeshVertexPosition( mesh, 2, { 20, 0, 0 } );
	PlgSetMeshVertexPosition( mesh, 3, { -20, 0, 0 } );
	PlgSetMeshVertexPosition( mesh, 4, { 0, 0, 20 } );
	PlgSetMeshVertexPosition( mesh, 5, { 0, 0, -20 } );
	PlgSetMeshUniformColour( mesh, hei::Colour( 255, 0, 0, 255 ) );

	ShaderProgram *shaderProgram = Shaders_GetProgram( "generic_untextured" );
	if ( shaderProgram == nullptr ) {
		Error( "Failed to get default shader program, \"generic_untextured\"!\n" );
	}

	// todo: kill this api, if we rebuild shader cache we'll die
	PlgSetMeshShaderProgram( mesh, shaderProgram->GetInternalProgram() );
	PlgUploadMesh( mesh );

	return ( fallbackModel = PlmCreateBasicStaticModel( mesh ) );
}

/**
 * Clear the specified resource if it's ready to be freed. Use force to immediately destroy it.
 */
void ohw::ResourceManager::ClearResource( const std::string &path, bool force ) {
	if ( resourcesMap.empty() ) {
		return;
	}

	auto resourceIndex = resourcesMap.find( path );
	if ( resourceIndex == resourcesMap.end() ) {
		return;
	}

	if ( !force && !resourceIndex->second->CanDestroy() ) {
		return;
	}

	delete resourceIndex->second;
	resourcesMap.erase( resourceIndex );

	DebugMsg( "Freed resource \"%s\"\n", path.c_str() );
}

/**
 * Clear all cached resources that can be freed up. Use force to immediately destroy it.
 */
void ohw::ResourceManager::ClearAllResources( bool force ) {
	for ( auto i = resourcesMap.begin(); i != resourcesMap.end(); ) {
		if ( !force && !i->second->CanDestroy() ) {
			++i;
			continue;
		}

		delete i->second;
		i = resourcesMap.erase( i );
	}
}

void ohw::ResourceManager::ListCachedResources( unsigned int argc, char **argv ) {
	u_unused( argc );
	u_unused( argv );

	Print( "Printing cache...\n" );
	for ( auto const &i: GetApp()->resourceManager->resourcesMap ) {
		Print(
				" CachedName(%s)"
				" CanDestroy(%s)"
				" ReferenceCount(%u)\n",
				i.first.c_str(),
				i.second->CanDestroy() ? "true" : "false",
				i.second->GetReferenceCount() );
	}
}

void ohw::ResourceManager::ClearAllResourcesCommand( unsigned int argc, char **argv ) {
	bool force = false;
	if ( argc > 1 ) {
		const char *forceParameter = argv[ 1 ];
		if ( forceParameter == nullptr ) {
			Warning( "Invalid force parameter!\n" );
			return;
		}

		if ( pl_strcasecmp( forceParameter, "true" ) == 0 ) {
			force = true;
		} else if ( pl_strcasecmp( forceParameter, "false" ) != 0 ) {
			Warning( "Invalid force parameter, should be \"true\" or \"false\", was \"%s\"!\n", forceParameter );
		}
	}

	GetApp()->resourceManager->ClearAllResources( force );
}

void ohw::ResourceManager::ClearResourceCommand( unsigned int argc, char **argv ) {
	if ( argc <= 1 ) {
		Warning( "Invalid number of arguments!\n" );
		return;
	}

	const char *resourceName = argv[ 1 ];
	if ( resourceName == nullptr ) {
		Warning( "Invalid resource name!\n" );
		return;
	}

	bool force = false;
	if ( argc > 2 ) {
		const char *forceParameter = argv[ 2 ];
		if ( forceParameter == nullptr ) {
			Warning( "Invalid force parameter!\n" );
			return;
		}

		if ( pl_strcasecmp( forceParameter, "true" ) == 0 ) {
			force = true;
		} else if ( pl_strcasecmp( forceParameter, "false" ) != 0 ) {
			Warning( "Invalid force parameter, should be \"true\" or \"false\", was \"%s\"!\n", forceParameter );
		}
	}

	GetApp()->resourceManager->ClearResource( resourceName, force );
}

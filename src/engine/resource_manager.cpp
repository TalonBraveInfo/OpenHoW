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
#include "ModelResource.h"
#include "TextureResource.h"
#include "resource_manager.h"
#include "graphics/shaders.h"

using namespace ohw;

ResourceManager::ResourceManager() {
	PLModel *LoadObjModel( const char *path ); // see loaders/obj.cpp
	plRegisterModelLoader( "obj", LoadObjModel );
	PLModel *Model_LoadVtxFile( const char *path );
	plRegisterModelLoader( "vtx", Model_LoadVtxFile );
	PLModel *Model_LoadMinFile( const char *path );
	plRegisterModelLoader( "min", Model_LoadMinFile );

	// Allow users to enable support for all model formats if desired (disabled by default for security reasons)
	if ( plHasCommandLineArgument( "-ramf" ) ) {
		plRegisterStandardModelLoaders();
	}

	// Allow users to enable support for all package formats if desired (disabled by default for security reasons)
	if ( plHasCommandLineArgument( "-rapf" ) ) {
		plRegisterStandardPackageLoaders();
	}

	plRegisterConsoleCommand( "ListCachedResources", &ResourceManager::ListCachedResources, "List all cached resources." );
	plRegisterConsoleCommand( "ClearModels", &ResourceManager::ClearModelsCommand, "Clears all cached models." );
	plRegisterConsoleCommand( "ClearTextures", &ResourceManager::ClearTexturesCommand, "Clears all cached textures." );
}

ResourceManager::~ResourceManager() {
	ClearAllResources( true );
}


//const char *supported_audio_formats[]={"wav", NULL};
//const char *supported_video_formats[]={"bik", NULL};

Resource *ResourceManager::GetCachedResource( const std::string& path ) {
	auto idx = resourcesMap.find( path );
	if ( idx != resourcesMap.end() ) {
		return idx->second;
	}

	return nullptr;
}

TextureResource *ResourceManager::LoadTexture( const std::string& path, PLTextureFilter filter, bool persist, bool abortOnFail ) {
	TextureResource *texturePtr = static_cast< TextureResource* >( GetCachedResource( path ) );
	if ( texturePtr != nullptr ) {
		return texturePtr;
	}

	texturePtr = new TextureResource( path, filter, persist, abortOnFail );
	CacheResource( path, texturePtr );

	return texturePtr;
}

ModelResource* ResourceManager::LoadModel( const std::string& path, bool persist, bool abortOnFail ) {
	ModelResource *modelPtr = static_cast< ModelResource* >( GetCachedResource( path ) );
	if ( modelPtr != nullptr ) {
		return modelPtr;
	}

	modelPtr = new ModelResource( path, persist, abortOnFail );
	CacheResource( path, modelPtr );

	return modelPtr;
}

PLTexture *ResourceManager::GetFallbackTexture() {
	if ( fallbackTexture != nullptr ) {
		return fallbackTexture;
	}

	PLColour pixelBuffer[] = {
		{ 255, 255, 0, 255 }, { 0, 255, 255, 255 },
		{ 0, 255, 255, 255 }, { 255, 255, 0, 255 } };
	PLImage *image = plCreateImage(( uint8_t* ) pixelBuffer, 2, 2, PL_COLOURFORMAT_RGBA, PL_IMAGEFORMAT_RGBA8 );
	if ( image == nullptr ) {
		Error( "Failed to generate default texture (%s)!\n", plGetError() );
	}

	fallbackTexture = plCreateTexture();
	fallbackTexture->flags &= PL_TEXTURE_FLAG_NOMIPS;
	if ( !plUploadTextureImage( fallbackTexture, image ) ) {
		Error( "Failed to upload default texture (%s)!\n", plGetError() );
	}

	plFreeImage( image );

	return fallbackTexture;
}

PLModel *ResourceManager::GetFallbackModel() {
	if ( fallbackModel != nullptr ) {
		return fallbackModel;
	}

	PLMesh* mesh = plCreateMesh( PL_MESH_LINES, PL_DRAW_DYNAMIC, 0, 6 );
	plSetMeshVertexPosition( mesh, 0, PLVector3( 0, 20, 0 ) );
	plSetMeshVertexPosition( mesh, 1, PLVector3( 0, -20, 0 ) );
	plSetMeshVertexPosition( mesh, 2, PLVector3( 20, 0, 0 ) );
	plSetMeshVertexPosition( mesh, 3, PLVector3( -20, 0, 0 ) );
	plSetMeshVertexPosition( mesh, 4, PLVector3( 0, 0, 20 ) );
	plSetMeshVertexPosition( mesh, 5, PLVector3( 0, 0, -20 ) );
	plSetMeshUniformColour( mesh, PLColour( 255, 0, 0, 255 ) );

	ShaderProgram* shaderProgram = Shaders_GetProgram( "generic_untextured" );
	if ( shaderProgram == nullptr ) {
		Error( "Failed to get default shader program, \"generic_untextured\"!\n" );
	}

	// todo: kill this api, if we rebuild shader cache we'll die
	plSetMeshShaderProgram( mesh, shaderProgram->GetInternalProgram() );
	plUploadMesh( mesh );

	return ( fallbackModel = plCreateBasicStaticModel( mesh ) );
}

void ResourceManager::ClearResource( const std::string &path, bool force ) {
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
}

void ResourceManager::ClearAllResources( bool force ) {
	for ( const auto& i : resourcesMap ) {
		if ( !force && !i.second->CanDestroy() ) {
			continue;
		}

		delete i.second;
		resourcesMap.erase( i );
	}
}

void ResourceManager::ListCachedResources( unsigned int argc, char** argv ) {
	u_unused( argc );
	u_unused( argv );

	LogInfo( "Printing cache...\n" );
	for ( auto const& i : Engine::Resource()->resourcesMap ) {
		LogInfo(
			" CachedName(%s)"
            " CanDestroy(%s)"
            " ReferenceCount(%u)\n",
            i.first.c_str(),
            i.second->CanDestroy() ? "true" : "false",
            i.second->GetReferenceCount() );
	}
}

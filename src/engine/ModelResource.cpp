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

#include "App.h"
#include "engine.h"
#include "ModelResource.h"
#include "TextureAtlas.h"
#include "mesh.h"
#include "WaveFrontReader.h"
#include "graphics/Camera.h"

#include "loaders/VtxLoader.h"
#include "loaders/FacLoader.h"
#include "loaders/No2Loader.h"

ohw::ModelResource::ModelResource( const std::string &path, bool persist, bool abortOnFail ) :
		Resource( path, persist ) {
	const char *fileExt = plGetFileExtension( path.c_str() );
	if ( fileExt == nullptr ) {
		if ( abortOnFail ) {
			Error( "Failed to fetch file extension, \"%s\"!\n", path.c_str() );
		}

		Warning( "Failed to fetch file extension, \"%s\"!\n", path.c_str() );
		return;
	}

	if ( pl_strcasecmp( fileExt, "vtx" ) == 0 ) {
		// It's a vtx model, so we'll be expecting a Fac and No2 as well
		LoadVtxModel( path, abortOnFail );
	} else if ( pl_strcasecmp( fileExt, "min" ) == 0 ) {
		// Min model format is used for the PSX version
		LoadMinModel( path, abortOnFail );
	} else if ( pl_strcasecmp( fileExt, "obj" ) == 0 ) {
		// Static Obj model format
		LoadObjModel( path, abortOnFail );
	} else {
		if ( abortOnFail ) {
			Error( "Unrecognised file extension, \"%s\"!\n", fileExt );
		}

		Warning( "Unrecognised file extension, \"%s\"!\n", fileExt );
		return;
	}

	GenerateBounds();
}

ohw::ModelResource::~ModelResource() {
	DestroyMeshes();
}

void ohw::ModelResource::Tick() {
	// TODO: animation is handled here...
}

void ohw::ModelResource::Draw( bool batchDraw ) {
	Camera *camera = Engine::Game()->GetCamera();
	if ( camera == nullptr ) {
		return;
	}

	bounds.origin = plGetMatrix4Translation( &modelMatrix );
	if ( cv_graphics_cull->b_value && !camera->IsBoxVisible( &bounds ) ) {
		return;
	}

	if ( cv_debug_bounds->b_value ) {
		plDrawBoundingVolume( &bounds, PL_COLOUR_BLUE );
	}

	if ( meshesVector.empty() ) {
		PLModel *model = Engine::Resource()->GetFallbackModel();
		if ( model == nullptr ) {
			return;
		}

		model->model_matrix = modelMatrix;

		plDrawModel( model );
		g_state.gfx.numModelsDrawn++;
		return;
	}

	// If specified, just add it to our list and return
	if ( batchDraw ) {
		batchedDrawCalls.push_back( modelMatrix );
		return;
	}

	for ( unsigned int i = 0; i < meshesVector.size(); ++i ) {
		DrawMesh( i );
	}

	if ( cv_graphics_debug_normals->b_value ) {
		DrawNormals();
	}

	g_state.gfx.numModelsDrawn++;
}

PLMesh *ohw::ModelResource::GetInternalMesh( unsigned int i ) {
	u_assert( i < meshesVector.size() );
	if ( i >= meshesVector.size() ) {
		Warning( "Attempted to access an invalid mesh (%d/%d)!\n", i, meshesVector.size() );
		return nullptr;
	}

	return meshesVector[ i ];
}

ohw::TextureResource *ohw::ModelResource::GetTextureResource(unsigned int i) {
	u_assert( i < texturesVector.size() );
	if ( i >= texturesVector.size() ) {
		Warning( "Attempted to access an invalid texture (%d/%d)!\n", i, texturesVector.size() );
		return nullptr;
	}

	return texturesVector[ i ];
}

void ohw::ModelResource::LoadObjModel( const std::string &path, bool abortOnFail ) {
	WaveFrontReader obj;
	if ( !obj.Load( path, true ) ) {
		return;
	}

	// Single material Objs are really damn easy here...
	if ( obj.materials.size() <= 2 ) {
		PLMesh *mesh = plCreateMesh( PL_MESH_TRIANGLES, PL_DRAW_STATIC, obj.indices.size(), obj.vertices.size() );
		if ( mesh == nullptr ) {
			if ( abortOnFail ) {
				Error( "Failed to create mesh! (%s)\n", plGetError() );
			}

			Warning( "Failed to create mesh! (%s)\n", plGetError() );
			return;
		}

		static_assert( sizeof( *mesh->indices ) == sizeof( *obj.indices.data() ), "mismatch" );
		// Copy all of the data across
		memcpy( mesh->indices, obj.indices.data(), sizeof( unsigned int ) * obj.indices.size() );
		memcpy( mesh->vertices, obj.vertices.data(), sizeof( PLVertex ) * obj.vertices.size() );

		std::string textureName = obj.materials[ obj.attributes[ 0 ] ].strTexture;
		if ( obj.materials.size() > 1 && !textureName.empty() ) {
			SharedTextureResourcePointer texture = Engine::Resource()->LoadTexture( textureName );
			texturesVector.push_back( texture );

			mesh->texture = texture->GetInternalTexture();
		} else {
			mesh->texture = Engine::Resource()->GetFallbackTexture();
		}

		// Push the mesh into our vector
		meshesVector.push_back( mesh );

		// And done!
		return;
	}

	// However we need to do some extra work for those that use multiple materials...

	struct MeshSet {
		std::vector< unsigned int > indices;
		WaveFrontReader::Material material;
	};
	std::map< unsigned int, MeshSet > meshSets;

	for ( unsigned int i = 0; i < obj.indices.size(); ++i ) {
		meshSets[ obj.attributes[ i / 3 ] ].indices.push_back( obj.indices[ i ] );
		meshSets[ obj.attributes[ i / 3 ] ].material = obj.materials[ obj.attributes[ i / 3 ] ];
	}

	meshesVector.resize( meshSets.size() );

	auto j = meshSets.begin();
	for ( unsigned int i = 0; j != meshSets.end(); ++j ) {
		meshesVector[ i ] = plCreateMesh( PL_MESH_TRIANGLES, PL_DRAW_STATIC, j->second.indices.size(),
		                                  obj.vertices.size() );
		if ( meshesVector[ i ] == nullptr ) {
			if ( abortOnFail ) {
				Error( "Failed to create mesh!\nPL: %s\n", plGetError() );
			}

			// Destroy all the attached meshes
			DestroyMeshes();

			Warning( "Failed to create mesh!\nPL: %s\n", plGetError() );
			return;
		}

		memcpy( meshesVector[ i ]->vertices, obj.vertices.data(), sizeof( PLVertex ) * obj.vertices.size() );
		memcpy( meshesVector[ i ]->indices, j->second.indices.data(),
		        sizeof( unsigned int ) * j->second.indices.size() );

		if ( !j->second.material.strTexture.empty() ) {
			SharedTextureResourcePointer texture = Engine::Resource()->LoadTexture( j->second.material.strTexture );
			texturesVector.push_back( texture );

			meshesVector[ i ]->texture = texture->GetInternalTexture();
		} else {
			meshesVector[ i ]->texture = Engine::Resource()->GetFallbackTexture();
		}

		++i;
	}

	// Done!
}

static TextureAtlas *
ModelResource_GenerateVtxTextureAtlas( const FacHandle *facHandle, const std::string &texturePath ) {
	if ( facHandle->texture_table_size == 0 ) {
		Warning( "Empty texture table!\n" );
		return nullptr;
	}

	TextureAtlas *atlas = new TextureAtlas( 128, 128 );

	for ( unsigned int i = 0; i < facHandle->texture_table_size; ++i ) {
		if ( facHandle->texture_table[ i ].name[ 0 ] == '\0' ) {
			Warning( "Invalid texture name in table, skipping (%d)!\n", i );
			continue;
		}

		std::string str = texturePath;
		std::string texture_path = str.erase( str.find_last_of( '/' ) ) + "/";
		if ( !atlas->AddImage( texture_path + facHandle->texture_table[ i ].name + ".png", true ) ) {
			Warning( "Failed to add texture \"%s\" to atlas!\n", facHandle->texture_table[ i ].name );
		}
	}

	atlas->Finalize();

	return atlas;
}

/**
 * Loader for Hogs of War's PC model format
 */
void ohw::ModelResource::LoadVtxModel( const std::string &path, bool abortOnFail ) {
	// Load in the vertices
	VtxHandle *vtxHandle = Vtx_LoadFile( path.c_str() );
	if ( vtxHandle == nullptr ) {
		if ( abortOnFail ) {
			Error( "Failed to load Vtx, \"%s\"!\n", path.c_str() );
		}

		Warning( "Failed to load Vtx, \"%s\"!\n", path.c_str() );
		return;
	}

	// Load in the faces
	char facesPath[PL_SYSTEM_MAX_PATH];
	u_new_filename( facesPath, path.c_str(), "fac" );
	FacHandle *facHandle = Fac_LoadFile( facesPath );
	if ( facHandle == nullptr ) {
		if ( abortOnFail ) {
			Error( "Failed to load Fac, \"%s\"!\n", facesPath );
		}

		Vtx_DestroyHandle( vtxHandle );

		Warning( "Failed to load Fac, \"%s\"!\n", facesPath );
		return;
	}

	// There are some special cases, so let's go ahead and deal with those...
	const char *fileName = plGetFileName( path.c_str() );
	if ( pl_strcasecmp( fileName, "skydome.vtx" ) == 0 || pl_strcasecmp( fileName, "skydomeu.vtx" ) == 0 ) {
		PLMesh *mesh = plCreateMesh( PL_MESH_TRIANGLES, PL_DRAW_STATIC, facHandle->num_triangles,
		                             vtxHandle->num_vertices );
		if ( mesh == nullptr ) {
			if ( abortOnFail ) {
				Error( "Failed to create mesh!\nPL: %s\n", plGetError() );
			}

			Vtx_DestroyHandle( vtxHandle );
			Fac_DestroyHandle( facHandle );

			Warning( "Failed to create mesh!\nPL: %s\n", plGetError() );
			return;
		}

		// Now we can set up our mesh

		for ( unsigned int j = 0; j < vtxHandle->num_vertices; ++j ) {
			plSetMeshVertexPosition( mesh, j, vtxHandle->vertices[ j ].position * -1.0f * 0.5f );
		}

		unsigned int cur_index = 0;
		for ( unsigned int j = 0; j < facHandle->num_triangles; ++j ) {
			plSetMeshTrianglePosition( mesh, &cur_index,
			                           facHandle->triangles[ j ].vertex_indices[ 0 ],
			                           facHandle->triangles[ j ].vertex_indices[ 1 ],
			                           facHandle->triangles[ j ].vertex_indices[ 2 ]
			);
		}

		// All done, throw these out now
		Vtx_DestroyHandle( vtxHandle );
		Fac_DestroyHandle( facHandle );

		mesh->texture = Engine::Resource()->GetFallbackTexture();

		meshesVector.push_back( mesh );
		return;
	}

	PLMesh *mesh = plCreateMesh( PL_MESH_TRIANGLES, PL_DRAW_STATIC, facHandle->num_triangles,
	                             facHandle->num_triangles * 3 );
	if ( mesh == nullptr ) {
		if ( abortOnFail ) {
			Error( "Failed to create mesh!\nPL: %s\n", plGetError() );
		}

		Vtx_DestroyHandle( vtxHandle );
		Fac_DestroyHandle( facHandle );

		Warning( "Failed to create mesh!\nPL: %s\n", plGetError() );
		return;
	}

	// Attempt to load in the normals, it's fine if these don't successfully load as we'll just generate them instead later
	char normalsPath[PL_SYSTEM_MAX_PATH];
	u_new_filename( normalsPath, path.c_str(), "no2" );
	No2Handle *no2Handle = No2_LoadFile( normalsPath );

	// Need to scale the model up, as the models are actually a little bit smaller than our terrain :(
	for ( unsigned int j = 0; j < vtxHandle->num_vertices; ++j ) {
		vtxHandle->vertices[ j ].position *= 0.5f;
	}

	// automatically returns default if failed
	std::string texturePath = facesPath;
	// Temporary hack just to get the pig textures loaded
	if ( strstr( facesPath, "pigs" ) != nullptr ) {
		texturePath = "chars/pigs/british/";
	}

	// Now create the atlas itself
	TextureAtlas *textureAtlas = ModelResource_GenerateVtxTextureAtlas( facHandle, texturePath );

	unsigned int curIndex = 0;
	for ( unsigned int i = 0, nextVtxIndex = 0; i < facHandle->num_triangles; ++i ) {
		for ( unsigned int triVtxIndex = 0; triVtxIndex < 3; ++triVtxIndex, ++nextVtxIndex ) {
			unsigned int triVtx = facHandle->triangles[ i ].vertex_indices[ triVtxIndex ];

			plSetMeshVertexColour( mesh, nextVtxIndex, PL_COLOUR_WHITE );
			plSetMeshVertexPosition( mesh, nextVtxIndex, vtxHandle->vertices[ triVtx ].position );

			if ( no2Handle != nullptr ) {
				unsigned int normalIndex = facHandle->triangles[ i ].normal_indices[ triVtxIndex ];
				plSetMeshVertexNormal( mesh, nextVtxIndex, no2Handle->normals[ normalIndex ] );
			}

			mesh->vertices[ nextVtxIndex ].bone_index = vtxHandle->vertices[ triVtx ].bone_index;
			mesh->vertices[ nextVtxIndex ].bone_weight = 1.0f;
		}

		plSetMeshTrianglePosition( mesh, &curIndex, nextVtxIndex - 1, nextVtxIndex - 2, nextVtxIndex - 3 );

		if ( facHandle->texture_table != nullptr && textureAtlas != nullptr ) {
			const char *textureName = facHandle->texture_table[ facHandle->triangles[ i ].texture_index ].name;

			float tX, tY, tW, tH;
			textureAtlas->GetTextureCoords( textureName, &tX, &tY, &tW, &tH );

			std::pair< unsigned int, unsigned int > textureSize = textureAtlas->GetTextureSize( textureName );

			for ( unsigned int j = 0, u = 0; j < 3; ++j, u += 2 ) {
				plSetMeshVertexST( mesh, nextVtxIndex - ( 3 - j ),
				                   tX + ( tW * ( 1.0f / ( float ) ( textureSize.first ) ) *
				                          ( float ) ( facHandle->triangles[ i ].uv_coords[ u ] ) ),
				                   tY + ( tH * ( 1.0f / ( float ) ( textureSize.second ) ) *
				                          ( float ) ( facHandle->triangles[ i ].uv_coords[ u + 1 ] ) ) );
			}
		}
	}

	if ( textureAtlas != nullptr ) {
		// TODO: we have no way of cleaning this up right now...
		mesh->texture = textureAtlas->GetTexture();
	} else {
		mesh->texture = Engine::Resource()->GetFallbackTexture();
	}

	delete textureAtlas;

	// Normals weren't loaded in, so attempt to generate them
#if 0
	if ( no2Handle == nullptr ) {
		std::list< PLMesh * > meshes( &mesh, &mesh + 1 );
		Mesh_GenerateFragmentedMeshNormals( meshes );
	} else {
		No2_DestroyHandle( no2Handle );
	}
#else
	// Always generate normals until we handle No2 correctly
	std::list< PLMesh * > meshes( &mesh, &mesh + 1 );
	Mesh_GenerateFragmentedMeshNormals( meshes );
#endif

	meshesVector.push_back( mesh );

	// Done!
}

/**
 * Loader for Hogs of War's PSX model format
 */
void ohw::ModelResource::LoadMinModel( const std::string &path, bool abortOnFail ) {
	u_assert( 0, "TODO" );
}

/**
 * Draws the specified mesh.
 */
void ohw::ModelResource::DrawMesh( unsigned int i ) {
	u_assert( i < meshesVector.size() );
	if ( i >= meshesVector.size() ) {
		Error( "Attempted to access an invalid mesh (%d/%d)!\n", i, meshesVector.size() );
	}

	PLShaderProgram *program = plGetCurrentShaderProgram();
	if ( program == nullptr ) {
		Error( "No bound shader program when drawing mesh %d!\n", i );
	}

	// TODO: This currently doesn't handle animations...
	// TODO: We should be batching the same things too!

	plSetTexture( meshesVector[ i ]->texture, 0 );

	plSetShaderUniformValue( program, "pl_model", &modelMatrix, true );

	plUploadMesh( meshesVector[ i ] );
	plDrawMesh( meshesVector[ i ] );
}

/**
 * Draws all the normals per mesh.
 */
void ohw::ModelResource::DrawNormals() {
	for ( auto i : meshesVector ) {
		plDrawMeshNormals( &modelMatrix, i );
	}
}

/**
 * Draw the model's skeleton if it's animated.
 */
void ohw::ModelResource::DrawSkeleton() {
	if ( !isAnimated ) {
		return;
	}

	// TODO: draw the bones...
}

void ohw::ModelResource::DestroyMeshes() {
	for ( auto mesh : meshesVector ) {
		if ( mesh == nullptr ) {
			continue;
		}

		plDestroyMesh( mesh );
	}

	meshesVector.clear();
	meshesVector.shrink_to_fit();
}

/**
 * Generate a bounding volume for the model. Used for visibility culling.
 */
void ohw::ModelResource::GenerateBounds() {
	if ( meshesVector.empty() ) {
		return;
	}

	// Gather all the vertices from every mesh
	std::vector< PLVertex > vertices;
	for ( const auto &mesh : meshesVector ) {
		vertices.insert( vertices.end(), mesh->vertices, mesh->vertices + mesh->num_verts );
	}

	// And now generate the bounds
	bounds = plGenerateAABB( vertices.data(), vertices.size(), false );

	// TODO: handle animations somehow? We might need to call this for each animation frame for animated models...
}

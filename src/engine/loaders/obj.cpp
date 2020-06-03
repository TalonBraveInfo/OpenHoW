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

#include "../engine.h"
#include "../graphics/display.h"

#include "WaveFrontReader.h"

using namespace ohw;

PLModel *LoadObjModel( const char *path ) {
	WaveFrontReader obj;
	if ( !obj.Load( path, true ) ) {
		return nullptr;
	}

	// Single material Objs are really damn easy here...
	if ( obj.materials.size() <= 2 ) {
		PLMesh *mesh = plCreateMesh( PL_MESH_TRIANGLES, PL_DRAW_STATIC, obj.indices.size(), obj.vertices.size() );
		if ( mesh == nullptr ) {
			LogWarn( "Failed to create mesh! (%s)\n", plGetError() );
			return nullptr;
		}

		static_assert( sizeof( *mesh->indices ) == sizeof( *obj.indices.data() ), "mismatch" );
		memcpy( mesh->indices, obj.indices.data(), sizeof( unsigned int ) * obj.indices.size() );
		memcpy( mesh->vertices, obj.vertices.data(), sizeof( PLVertex ) * obj.vertices.size() );

		if ( obj.materials.size() > 1 ) {
			// TODO: oh dear... Oh dear dear... Reference will get released - we can't parent this to the meshes (need wrapper class) -
			//  in the mean time this'll have to leak :(
			SharedTextureResourcePointer texture = Engine::Resource()->LoadTexture( obj.materials[ obj.attributes[ 0 ] ].strTexture, PL_TEXTURE_FILTER_MIPMAP_LINEAR );
			texture->AddReference();

			mesh->texture = texture->GetInternalTexture();
		} else {
			mesh->texture = Engine::Resource()->GetFallbackTexture();
		}

		return plCreateBasicStaticModel( mesh );
	}

	// However we need to do some extra work for those that use multiple materials...

	struct MeshSet {
		std::vector<unsigned int> indices;
		WaveFrontReader::Material material;
	};
	std::map<unsigned int, MeshSet> meshSets;

	for ( unsigned int i = 0; i < obj.indices.size(); ++i ) {
		meshSets[ obj.attributes[ i / 3 ] ].indices.push_back( obj.indices[ i ] );
		meshSets[ obj.attributes[ i / 3 ] ].material = obj.materials[ obj.attributes[ i / 3 ] ];
	}

	PLMesh **meshes = static_cast<PLMesh **>(malloc( meshSets.size() * sizeof( PLMesh * ) ));
	auto j = meshSets.begin();
	for ( unsigned int i = 0; j != meshSets.end(); ++j ) {
		meshes[ i ] = plCreateMesh( PL_MESH_TRIANGLES, PL_DRAW_STATIC, j->second.indices.size(), obj.vertices.size() );
		if ( meshes[ i ] == nullptr ) {
			LogWarn( "Failed to create mesh! (%s)\n", plGetError() );
			return nullptr;
		}

		memcpy( meshes[ i ]->vertices, obj.vertices.data(), sizeof( PLVertex ) * obj.vertices.size() );
		memcpy( meshes[ i ]->indices, j->second.indices.data(), sizeof( unsigned int ) * j->second.indices.size() );

		// TODO: oh dear... Oh dear dear... Reference will get released - we can't parent this to the meshes (need wrapper class) -
		//  in the mean time this'll have to leak :(
		SharedTextureResourcePointer texture = Engine::Resource()->LoadTexture( j->second.material.strTexture, PL_TEXTURE_FILTER_MIPMAP_LINEAR );
		texture->AddReference();

		meshes[ i ]->texture = texture->GetInternalTexture();

		++i;
	}

	PLModelLod lod;
	lod.meshes = meshes;
	lod.num_meshes = meshSets.size();

	PLModel *model = plCreateStaticModel( &lod, 1 );
	if ( model == nullptr ) {
		Error( "Failed to create model container! (%s)\n", plGetError() );
	}

	return model;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include <map>
#include <set>

#include "mesh.h"

void Mesh_GenerateFragmentedMeshNormals( const std::list< PLGMesh * > &meshes ) {
	struct Position {
		hei::Vector3 sum_normals;
		std::set< PLGVertex * > vertices;
		unsigned int num_faces;

		Position( const hei::Vector3 &normal, PLGVertex *output ) :
				sum_normals( normal ), num_faces( 1 ) {
			vertices.insert( output );
		}
	};

	std::map< hei::Vector3, Position > positions;

	for ( auto &mesh: meshes ) {
		for ( unsigned int i = 0, idx = 0; i < mesh->num_triangles; ++i, idx += 3 ) {
			unsigned int a = mesh->indices[ idx ];
			unsigned int b = mesh->indices[ idx + 1 ];
			unsigned int c = mesh->indices[ idx + 2 ];

			PLVector3 normal = PlgGenerateVertexNormal(
					mesh->vertices[ a ].position,
					mesh->vertices[ b ].position,
					mesh->vertices[ c ].position
			);

			for ( unsigned int j = 0; j < 3; ++j ) {
				unsigned int vi = mesh->indices[ idx + j ];

				PLGVertex *vertex = &( mesh->vertices[ vi ] );

				auto ni = positions.find( vertex->position );
				if ( ni != positions.end() ) {
					ni->second.sum_normals += normal;
					ni->second.vertices.insert( vertex );
					++( ni->second.num_faces );
				} else {
					positions.insert( std::make_pair( vertex->position, Position( normal, vertex ) ) );
				}
			}
		}
	}

	for ( auto &position: positions ) {
		for ( PLGVertex *vertex: position.second.vertices ) {
			//vertex->normal = (position.second.sum_normals / position.second.num_faces).Normalize();
			vertex->normal = position.second.sum_normals / position.second.num_faces;
			//vertex->normal = position.second.sum_normals.Normalize();
		}
	}
}

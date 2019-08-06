/* OpenHoW
 * Copyright (C) 2019 Mark Sowden <markelswo@gmail.com>
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

#include <map>
#include <set>

#include <PL/platform_mesh.h>
#include <PL/pl_math_vector.h>

void Mesh_GenerateFragmentedMeshNormals(PLMesh *mesh) {
    plAssert(mesh);

    struct Foo {
        PLVector3 normal;
        std::set<unsigned int> vertex_indices;

        Foo(const PLVector3 &normal, unsigned int vertex_index):
            normal(normal) {
            vertex_indices.insert(vertex_index);
        }
    };

    std::map<PLVector3, Foo> normals;

    for (unsigned int i = 0, idx = 0; i < mesh->num_triangles; ++i, idx += 3) {
        unsigned int a = mesh->indices[idx];
        unsigned int b = mesh->indices[idx + 1];
        unsigned int c = mesh->indices[idx + 2];

        PLVector3 normal = plGenerateVertexNormal(
            mesh->vertices[a].position,
            mesh->vertices[b].position,
            mesh->vertices[c].position
        );

        for(unsigned int j = 0; j < 3; ++j) {
            unsigned int vi = mesh->indices[idx + j];

            auto ni = normals.find(mesh->vertices[vi].position);
            if(ni != normals.end()) {
                ni->second.normal = plAddVector3(ni->second.normal, normal);
                ni->second.vertex_indices.insert(vi);
            } else {
                normals.insert(std::make_pair(mesh->vertices[vi].position, Foo(normal, vi)));
            }
        }
    }

    for(auto ni = normals.begin(); ni != normals.end(); ++ni) {
        for(auto vii = ni->second.vertex_indices.begin(); vii != ni->second.vertex_indices.end(); ++vii) {
        mesh->vertices[*vii].normal = ni->second.normal;
        }
    }

    for(unsigned int i = 0; i < mesh->num_verts; ++i) {
        mesh->vertices[i].normal = plNormalizeVector3(mesh->vertices[i].normal);
    }
}

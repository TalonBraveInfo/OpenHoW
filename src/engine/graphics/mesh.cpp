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

#include <list>
#include <map>
#include <set>

#include <PL/platform_mesh.h>
#include <PL/pl_math_vector.h>

void Mesh_GenerateFragmentedMeshNormals(const std::list<PLMesh*>& meshes) {
    struct Position {
        PLVector3 sum_normals;
        std::set<PLVertex*> vertices;
        unsigned int num_faces;

        Position(const PLVector3 &normal, PLVertex *output):
            sum_normals(normal), num_faces(1) {
          vertices.insert(output);
        }
    };

    std::map<PLVector3, Position> positions;

    for (auto & mesh : meshes) {
      for (unsigned int i = 0, idx = 0; i < mesh->num_triangles; ++i, idx += 3) {
        unsigned int a = mesh->indices[idx];
        unsigned int b = mesh->indices[idx + 1];
        unsigned int c = mesh->indices[idx + 2];

        PLVector3 normal = plGenerateVertexNormal(
            mesh->vertices[a].position,
            mesh->vertices[b].position,
            mesh->vertices[c].position
        );

        for (unsigned int j = 0; j < 3; ++j) {
          unsigned int vi = mesh->indices[idx + j];

          PLVertex *vertex = &(mesh->vertices[vi]);

          auto ni = positions.find(vertex->position);
          if (ni != positions.end()) {
            ni->second.sum_normals += normal;
            ni->second.vertices.insert(vertex);
            ++(ni->second.num_faces);
          } else {
            positions.insert(std::make_pair(vertex->position, Position(normal, vertex)));
          }
        }
      }
    }

    for(auto &position : positions) {
        for(PLVertex *vertex : position.second.vertices) {
          // vertex->normal = (position.second.sum_normals / position.second.num_faces).Normalize();
          // vertex->normal = position.second.sum_normals / position.second.num_faces;
          vertex->normal = position.second.sum_normals.Normalize();
        }
    }
}

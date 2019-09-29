/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

using namespace openhow;

PLModel *LoadObjModel(const char *path) {
    WaveFrontReader obj;
    if(!obj.Load(path, false)) {
        return nullptr;
    }

    PLMesh *mesh = plCreateMesh(PL_MESH_TRIANGLES, PL_DRAW_STATIC, obj.indices.size(), obj.vertices.size());
    if(mesh == nullptr) {
        LogWarn("Failed to create mesh! (%s)\n", plGetError());
        return nullptr;
    }

    static_assert(sizeof(*mesh->indices) == sizeof(*obj.indices.data()), "mismatch");
    memcpy(mesh->indices, obj.indices.data(), sizeof(unsigned int) * obj.indices.size());
    memcpy(mesh->vertices, obj.vertices.data(), sizeof(PLVertex) * obj.vertices.size());

    plSetMeshUniformColour(mesh, PLColour(255, 255, 255));

    if(!obj.materials.empty()) {
        mesh->texture = Engine::ResourceManagerInstace()->LoadTexture(obj.materials[1].strTexture, PL_TEXTURE_FILTER_MIPMAP_NEAREST);
    } else {
        mesh->texture = Engine::ResourceManagerInstace()->GetFallbackTexture();
    }

    return plCreateBasicStaticModel(mesh);
}

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

#include "Resource.h"
#include "TextureResource.h"

namespace ohw {
	class ModelResource : public Resource {
	public:
		IMPLEMENT_RESOURCE_CLASS( ModelResource )

		explicit ModelResource( const std::string &path, bool persist = false, bool abortOnFail = false );
		~ModelResource();

		void Tick();
		void Draw();

		PL_INLINE unsigned int GetNumberOfMeshes() const { return meshesVector.size(); }
		PLMesh *GetInternalMesh( unsigned int i );

		const PLCollisionAABB &GetBounds() const { return bounds; }

		PLMatrix4 modelMatrix{};

	private:
		void LoadObjModel( const std::string &path, bool abortOnFail );
		void LoadVtxModel( const std::string &path, bool abortOnFail );
		void LoadMinModel( const std::string &path, bool abortOnFail );

		void DrawMesh( unsigned int i );
		void DrawNormals();
		void DrawSkeleton();

		void DestroyMeshes();

		void GenerateBounds();

		bool isAnimated{ false };

		// List of textures this model depends on
		std::vector< SharedTextureResourcePointer > texturesVector;
		std::vector< PLMesh* > meshesVector;

		PLCollisionAABB bounds;
	};

	using SharedModelResourcePointer = SharedResourcePointer< ModelResource >;
}

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

		void Draw( bool batchDraw = false );
		void DrawNormals();
		void DrawSkeleton();

		// Batching

		PL_INLINE void AddDrawToQueue( const PLMatrix4 &transform ) {
			batchedDrawCalls.push_back( transform );
		}

		PL_INLINE unsigned int GetNumberOfQueuedDraws() const {
			return batchedDrawCalls.size();
		}

		PL_INLINE void ClearQueuedDraws() {
			batchedDrawCalls.clear();
		}

		// Mesh state

		PL_INLINE unsigned int GetNumberOfMeshes() const { return meshesVector.size(); }
		PLMesh *GetInternalMesh( unsigned int i );

		const PLCollisionAABB &GetBounds() const { return bounds; }

		PL_INLINE bool IsAnimated() const { return isAnimated; }

		PLMatrix4 modelMatrix{};

	private:
		void LoadObjModel( const std::string &path, bool abortOnFail );
		void LoadVtxModel( const std::string &path, bool abortOnFail );
		void LoadMinModel( const std::string &path, bool abortOnFail );

		void DrawMesh( unsigned int i );

		void DestroyMeshes();

		void GenerateBounds();

		bool isAnimated{ false };

		std::vector< SharedTextureResourcePointer > texturesVector; // List of textures this model depends on

		std::vector< PLMesh * > meshesVector;       // Sub-meshes that are part of this model
		std::vector< PLMatrix4 > batchedDrawCalls;  // Draw queue. Anything queued up will be pushed to the GPU in one batch

		PLCollisionAABB bounds;
	};

	using SharedModelResourcePointer = SharedResourcePointer< ModelResource >;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include "Resource.h"
#include "TextureResource.h"

namespace ohw {
	class ModelResource : public Resource {
	public:
		IMPLEMENT_RESOURCE_CLASS( ModelResource )

		explicit ModelResource( const std::string &path, bool persist = false, bool abortOnFail = false );
		~ModelResource();

		struct Keyframe {
			PLVector3 transforms[10];
			PLQuaternion rotations[15];
		};

		struct Animation {
			unsigned int id;
			const char *name;

			Keyframe *frames;
			unsigned int num_frames;
		};

		void Tick();

		void Draw( bool cull = true, bool batchDraw = false );
		void DrawNormals();
		void DrawSkeleton();

		// Batching

		inline void AddDrawToQueue( const PLMatrix4 &transform ) {
			batchedDrawCalls.push_back( transform );
		}

		inline unsigned int GetNumberOfQueuedDraws() const {
			return batchedDrawCalls.size();
		}

		inline void ClearQueuedDraws() {
			batchedDrawCalls.clear();
		}

		// Mesh state

		inline unsigned int GetNumberOfMeshes() const { return meshesVector.size(); }
		PLGMesh *GetInternalMesh( unsigned int i );

		const PLCollisionAABB &GetBounds() const { return bounds; }

		TextureResource *GetTextureResource( unsigned int i );

		inline bool IsAnimated() const { return isAnimated; }

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

		std::vector< PLGMesh * > meshesVector;       // Sub-meshes that are part of this model
		std::vector< PLMatrix4 > batchedDrawCalls;  // Draw queue. Anything queued up will be pushed to the GPU in one batch

		PLCollisionAABB bounds;
	};

	using SharedModelResourcePointer = SharedResourcePointer< ModelResource >;
}

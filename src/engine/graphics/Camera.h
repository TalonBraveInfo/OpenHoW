// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include <plgraphics/plg_camera.h>

namespace ohw {
	class Camera {
	public:
		Camera( const PLVector3 &pos, const PLVector3 &angles );
		~Camera();

		inline void SetPosition( const PLVector3 &pos ) { internalCameraPtr->position = pos; }
		inline void SetPosition( float x, float y, float z ) { internalCameraPtr->position = { x, y, z }; }

		inline void SetAngles( const PLVector3 &angles ) { internalCameraPtr->angles = angles; }
		inline void SetAngles( float x, float y, float z ) { internalCameraPtr->angles = { x, y, z }; }

		void SetFieldOfView( float fov );
		inline void SetNear( float near ) { internalCameraPtr->near = near; }
		inline void SetFar( float far ) { internalCameraPtr->far = far; }

		inline PLVector3 GetPosition() const { return internalCameraPtr->position; }
		inline PLVector3 GetAngles() const { return internalCameraPtr->angles; }
		inline PLVector3 GetForward() const { return internalCameraPtr->forward; }

		inline float GetFieldOfView() const { return internalCameraPtr->fov; }

		void SetViewport( int x, int y, int width, int height );

		inline int GetViewportWidth() const { return internalCameraPtr->viewport.w; }
		inline int GetViewportHeight() const { return internalCameraPtr->viewport.h; }

		void MakeActive();

		bool IsBoxVisible( const PLCollisionAABB *bounds ) const;
		bool IsSphereVisible( const PLCollisionSphere *sphere ) const;

	protected:
	private:
		PLGCamera *internalCameraPtr{ nullptr };
	};
}

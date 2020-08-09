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

#include <PL/pl_graphics_camera.h>

namespace ohw {
	class Camera {
	public:
		Camera( const PLVector3 &pos, const PLVector3 &angles );
		~Camera();

		PL_INLINE void SetPosition( const PLVector3 &pos ) { internalCameraPtr->position = pos; }
		PL_INLINE void SetPosition( float x, float y, float z ) { internalCameraPtr->position = PLVector3( x, y, z ); }

		PL_INLINE void SetAngles( const PLVector3 &angles ) { internalCameraPtr->angles = angles; }
		PL_INLINE void SetAngles( float x, float y, float z ) { internalCameraPtr->angles = PLVector3( x, y, z ); }

		void SetFieldOfView( float fov );
		PL_INLINE void SetNear( float near ) { internalCameraPtr->near = near; }
		PL_INLINE void SetFar( float far ) { internalCameraPtr->far = far; }

		PL_INLINE PLVector3 GetPosition() const { return internalCameraPtr->position; }
		PL_INLINE PLVector3 GetAngles() const { return internalCameraPtr->angles; }
		PL_INLINE PLVector3 GetForward() const { return internalCameraPtr->forward; }

		PL_INLINE float GetFieldOfView() const { return internalCameraPtr->fov; }

		void SetViewport( int x, int y, int width, int height );

		PL_INLINE int GetViewportWidth() const { return internalCameraPtr->viewport.w; }
		PL_INLINE int GetViewportHeight() const { return internalCameraPtr->viewport.h; }

		void MakeActive();

		bool IsBoxVisible( const PLCollisionAABB *bounds ) const;
		bool IsSphereVisible( const PLCollisionSphere *sphere ) const;

		void DrawViewFrustum();

	protected:
	private:
		PLCamera *internalCameraPtr{ nullptr };
	};
}

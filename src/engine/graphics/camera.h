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

		PL_INLINE void SetPosition( const PLVector3 &pos ) { camera_->position = pos; }
		PL_INLINE void SetAngles( const PLVector3 &angles ) { camera_->angles = angles; }
		PL_INLINE void SetFieldOfView( float fov ) { camera_->fov = fov; }
		PL_INLINE void SetNear( float near ) { camera_->near = near; }
		PL_INLINE void SetFar( float far ) { camera_->far = far; }

		PL_INLINE PLVector3 GetPosition() const { return camera_->position; }
		PL_INLINE PLVector3 GetAngles() const { return camera_->angles; }
		PL_INLINE PLVector3 GetForward() const { return camera_->forward; }

		PL_INLINE float GetFieldOfView() const { return camera_->fov; }

		void SetViewport( int x, int y, int width, int height );

		PL_INLINE int GetViewportWidth() const { return camera_->viewport.w; }
		PL_INLINE int GetViewportHeight() const { return camera_->viewport.h; }

		void MakeActive();

		bool IsBoxVisible( const PLCollisionAABB *bounds ) const;
		bool IsSphereVisible( const PLCollisionSphere *sphere ) const;

		void DrawViewFrustum();

	protected:
	private:
		static PLMesh *frustumPreviewMesh;

		PLCamera *camera_{ nullptr };
	};
}

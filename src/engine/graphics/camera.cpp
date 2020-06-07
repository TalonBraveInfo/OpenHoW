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

#include "engine.h"
#include "camera.h"

ohw::Camera::Camera( const PLVector3 &pos, const PLVector3 &angles ) {
	camera_ = plCreateCamera();
	if ( camera_ == nullptr ) {
		Error( "Failed to create camera object!\n%s\n", plGetError() );
	}

	camera_->mode = PL_CAMERA_MODE_PERSPECTIVE;
	camera_->fov = cv_camera_fov->f_value;
	camera_->far = cv_camera_far->f_value;
	camera_->near = cv_camera_near->f_value;
	camera_->viewport.w = cv_display_width->i_value;
	camera_->viewport.h = cv_display_height->i_value;
}

ohw::Camera::~Camera() {
	plDestroyCamera( camera_ );
}

void ohw::Camera::SetPosition( const PLVector3 &pos ) {
	camera_->position = pos;
}

void ohw::Camera::SetAngles( const PLVector3 &angles ) {
	camera_->angles = angles;
}

void ohw::Camera::SetFieldOfView( float fov ) {
	camera_->fov = fov;
}

/**
 * Sets the viewport position and size.
 * @param xy X and Y.
 * @param wh Width and height.
 */
void ohw::Camera::SetViewport( int x, int y, int width, int height ) {
	camera_->viewport.x = x;
	camera_->viewport.y = y;
	camera_->viewport.w = width;
	camera_->viewport.h = height;
}

void ohw::Camera::MakeActive() {
	// ensure camera matches current vars
	//camera_->fov = cv_camera_fov->f_value;
	camera_->near = cv_camera_near->f_value;
	camera_->far = cv_camera_far->f_value;

	plSetupCamera( camera_ );
}

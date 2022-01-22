// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Camera.h"

ohw::Camera::Camera( const PLVector3 &pos, const PLVector3 &angles ) {
	internalCameraPtr = PlgCreateCamera();
	if ( internalCameraPtr == nullptr ) {
		Error( "Failed to create camera object!\n%s\n", PlGetError() );
	}
}

ohw::Camera::~Camera() {
	PlgDestroyCamera( internalCameraPtr );
}

void ohw::Camera::SetFieldOfView( float fov ) {
	// Call the platform lib function for this,
	// which'll do some validation for us
	PlgSetCameraFieldOfView( internalCameraPtr, fov );
}

/**
 * Sets the viewport position and size.
 * @param xy X and Y.
 * @param wh Width and height.
 */
void ohw::Camera::SetViewport( int x, int y, int width, int height ) {
	internalCameraPtr->viewport.x = x;
	internalCameraPtr->viewport.y = y;
	internalCameraPtr->viewport.w = width;
	internalCameraPtr->viewport.h = height;
}

void ohw::Camera::MakeActive() {
	PlgSetupCamera( internalCameraPtr );
}

bool ohw::Camera::IsBoxVisible( const PLCollisionAABB *bounds ) const {
	return PlgIsBoxInsideView( internalCameraPtr, bounds );
}

bool ohw::Camera::IsSphereVisible( const PLCollisionSphere *sphere ) const {
	return PlgIsSphereInsideView( internalCameraPtr, sphere );
}

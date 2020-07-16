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

PLMesh *ohw::Camera::frustumPreviewMesh = nullptr;

ohw::Camera::Camera( const PLVector3 &pos, const PLVector3 &angles ) {
	camera_ = plCreateCamera();
	if ( camera_ == nullptr ) {
		Error( "Failed to create camera object!\n%s\n", plGetError() );
	}

	// If the preview mesh hasn't been generated yet, create it
	if ( frustumPreviewMesh == nullptr ) {
		frustumPreviewMesh = plCreateMesh( PL_MESH_LINES, PL_DRAW_DYNAMIC, 0, 8 );
		if ( frustumPreviewMesh == nullptr ) {
			Error( "Failed to create frustum mesh!\nPL: %s\n", plGetError() );
		}
	}
}

ohw::Camera::~Camera() {
	plDestroyCamera( camera_ );
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
	plSetupCamera( camera_ );
}

bool ohw::Camera::IsBoxVisible( const PLCollisionAABB *bounds ) const {
	return plIsBoxInsideView( camera_, bounds );
}

bool ohw::Camera::IsSphereVisible( const PLCollisionSphere *sphere ) const {
	return plIsSphereInsideView( camera_, sphere );
}

void ohw::Camera::DrawViewFrustum() {
	plClearMesh( frustumPreviewMesh );

	const PLViewFrustum &frustum = camera_->frustum;
	for ( unsigned int i = 0; i < 4; ++i ) {
		plAddMeshVertex( frustumPreviewMesh, PLVector3( frustum[ i ].x, frustum[ i ].y, frustum[ i ].z ) * frustum[ i ].w, PLVector3(), PL_COLOUR_RED, PLVector2() );
	}

	plSetNamedShaderUniformMatrix4( NULL, "pl_model", plMatrix4Identity(), false );

	plUploadMesh( frustumPreviewMesh );
	plDrawMesh( frustumPreviewMesh );
}

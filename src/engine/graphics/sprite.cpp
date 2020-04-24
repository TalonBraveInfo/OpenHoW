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

#include "../engine.h"

#include "sprite.h"
#include "display.h"
#include "shaders.h"

Sprite::Sprite( SpriteType type, PLTexture* texture, PLColour colour, float scale ) :
	type_( type ),
	colour_( colour ),
	scale_( scale ) {
	mesh_ = plCreateMeshRectangle( -64, -64, 64, 64, colour_ );
	mesh_->texture = texture;

	defaultProgram = Shaders_GetProgram( "generic_textured" );

	matrix_.Identity();
}

Sprite::~Sprite() = default;

void Sprite::Draw() {
	if ( !cv_graphics_draw_sprites->b_value ) {
		return;
	}

	if ( scale_ <= 0 ) {
		return;
	}

	defaultProgram->Enable();

	matrix_.Identity();
	matrix_ *= PLVector3( scale_, scale_, scale_ );
	matrix_.Translate( { 32 * scale_, 32 * scale_, 0 } );
	matrix_.Rotate( angles_.x, { 1, 0, 0 } );
	matrix_.Rotate( angles_.y, { 0, 1, 0 } );
	matrix_.Rotate( angles_.z, { 0, 0, 1 } );
	matrix_.Translate( position_ );

	//matrix_ = matrix_ * PLVector3( scale_, scale_, scale_ );

	plSetTexture( mesh_->texture, 0 );

	plSetNamedShaderUniformMatrix4( NULL, "pl_model", matrix_, true );

	plUploadMesh( mesh_ );

	plSetCullMode( PL_CULL_NONE );

	plDrawMesh( mesh_ );

	plSetCullMode( PL_CULL_POSTIVE );

	plSetTexture( NULL, 0 );
}

#if 0
void Sprite::SetAnimation(SpriteAnimation* anim) {
  u_assert(anim != nullptr, "Invalid pointer passed for animation!\n");
  current_frame_ = 0;
  current_animation_ = anim;
}
#endif

void Sprite::SetScale( float scale ) {
	scale_ = scale;
}

void Sprite::SetPosition( const PLVector3& position ) {
	position_ = position;
}

void Sprite::SetAngles( const PLVector3& angles ) {
	angles_ = angles;
}

void Sprite::SetColour( const PLColour& colour ) {
	plSetMeshUniformColour( mesh_, colour );
	colour_ = colour;
}

void Sprite::SetTexture( PLTexture* texture ) {
	// a lot of this will change once the rc manager is introduced...

	if ( texture == mesh_->texture ) {
		return;
	}

	mesh_->texture = texture;
}

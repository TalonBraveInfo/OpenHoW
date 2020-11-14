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

#include "App.h"
#include "Sprite.h"
#include "ShaderManager.h"

ohw::Sprite::Sprite( SpriteType type, const std::string &texturePath, PLColour colour, float scale ) :
		type_( type ), colour_( colour ), scale_( scale ) {
	mesh_ = plCreateMeshRectangle( -64, -64, 64, 64, colour_ );

	// Load in the texture we need
	texture = ohw::GetApp()->resourceManager->LoadTexture( texturePath );

	defaultProgram = Shaders_GetProgram( "generic_textured" );

	modelMatrix.Identity();
}

ohw::Sprite::~Sprite() {}

void ohw::Sprite::Draw() {
	if ( !cv_graphics_draw_sprites->b_value ) {
		return;
	}

	if ( scale_ <= 0 ) {
		return;
	}

	defaultProgram->Enable();

	plSetTexture( texture->GetInternalTexture(), 0 );

	modelMatrix.Identity();
	modelMatrix *= PLVector3( scale_, scale_, scale_ );
	modelMatrix.Translate( { 32 * scale_, 32 * scale_, 0 } );
	modelMatrix.Rotate( angles_.x, { 1, 0, 0 } );
	modelMatrix.Rotate( angles_.y, { 0, 1, 0 } );
	modelMatrix.Rotate( angles_.z, { 0, 0, 1 } );
	modelMatrix.Translate( position_ );

	plSetShaderUniformValue( defaultProgram->GetInternalProgram(), "pl_model", &modelMatrix, true );

	plUploadMesh( mesh_ );

	plSetCullMode( PL_CULL_NONE );

	plDrawMesh( mesh_ );

	plSetCullMode( PL_CULL_POSTIVE );

	plSetTexture( NULL, 0 );
}

#if 0
void ohw::Sprite::SetAnimation(SpriteAnimation* anim) {
  u_assert(anim != nullptr, "Invalid pointer passed for animation!\n");
  current_frame_ = 0;
  current_animation_ = anim;
}
#endif

void ohw::Sprite::SetScale( float scale ) {
	scale_ = scale;
}

void ohw::Sprite::SetPosition( const PLVector3 &position ) {
	position_ = position;
}

void ohw::Sprite::SetAngles( const PLVector3 &angles ) {
	angles_ = angles;
}

void ohw::Sprite::SetColour( const PLColour &colour ) {
	plSetMeshUniformColour( mesh_, colour );
	colour_ = colour;
}

void ohw::Sprite::SetTexture( const std::string &texturePath ) {
	if ( texture != nullptr ) {
		// TODO: this will currently fail (internal uses absolute)
		if ( texturePath == texture->GetInternalTexture()->path ) {
			return;
		}

		texture->Release();
	}

	texture = ohw::GetApp()->resourceManager->LoadTexture( texturePath );
}

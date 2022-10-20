// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Sprite.h"
#include "ShaderManager.h"

ohw::Sprite::Sprite( SpriteType type, const std::string &texturePath, PLColour colour, float scale ) :
		type_( type ), colour_( colour ), scale_( scale ) {
	mesh_ = PlgCreateMeshRectangle( -64, -64, 64, 64, colour_ );

	// Load in the texture we need
	texture = ohw::GetApp()->resourceManager->LoadTexture( texturePath );

	defaultProgram = Shaders_GetProgram( "generic_textured" );
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

	PlgSetTexture( texture->GetInternalTexture(), 0 );

	modelMatrix.Identity();
	modelMatrix *= hei::Vector3( scale_, scale_, scale_ );
	modelMatrix.Translate( { 32 * scale_, 32 * scale_, 0 } );
	modelMatrix.Rotate( angles_.x, { 1, 0, 0 } );
	modelMatrix.Rotate( angles_.y, { 0, 1, 0 } );
	modelMatrix.Rotate( angles_.z, { 0, 0, 1 } );
	modelMatrix.Translate( position_ );

	PlgSetShaderUniformValue( defaultProgram->GetInternalProgram(), "pl_model", &modelMatrix, true );

	PlgUploadMesh( mesh_ );

	PlgSetCullMode( PLG_CULL_NONE );

	PlgDrawMesh( mesh_ );

	PlgSetCullMode( PLG_CULL_POSITIVE );

	PlgSetTexture( nullptr, 0 );
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
	PlgSetMeshUniformColour( mesh_, colour );
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

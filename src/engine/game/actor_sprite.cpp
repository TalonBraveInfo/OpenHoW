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
#include "../graphics/sprite.h"

#include "actor_manager.h"
#include "actor.h"

// ASprite
// Can be used for very basic effects etc.

class ASprite : public Actor {
	IMPLEMENT_ACTOR( ASprite, Actor )

public:
	ASprite();
	~ASprite() override;

	void Tick() override;
	void Draw() override;

	virtual void SetSpriteTexture( const std::string &path );
	virtual void SetSpriteTexture( PLTexture *texture );

protected:
private:
	Sprite *sprite_;
};

REGISTER_ACTOR( sprite, ASprite )

using namespace ohw;

ASprite::ASprite() : SuperClass() {
	sprite_ = new Sprite( Sprite::TYPE_DEFAULT, Engine::Resource()->GetFallbackTexture() );
}

ASprite::~ASprite() {
	delete sprite_;
}

void ASprite::SetSpriteTexture( const std::string &path ) {
	SetSpriteTexture( Engine::Resource()->LoadTexture( path,
													   cv_graphics_texture_filter->b_value ? PL_TEXTURE_FILTER_LINEAR
																						   : PL_TEXTURE_FILTER_NEAREST ) );
}

void ASprite::SetSpriteTexture( PLTexture *texture ) {
	sprite_->SetTexture( texture );
}

void ASprite::Tick() {
	SuperClass::Tick();

	SetAngles( angles_.GetValue() + 0.2f );

	sprite_->SetPosition( position_ );
	sprite_->SetAngles( angles_ );
}

void ASprite::Draw() {
	SuperClass::Draw();

	sprite_->Draw();
}

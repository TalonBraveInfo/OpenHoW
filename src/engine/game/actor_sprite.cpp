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
#include "sprite.h"
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

protected:
private:
	Sprite *spritePtr;
};

REGISTER_ACTOR_BASIC( ASprite )

using namespace ohw;

ASprite::ASprite() : SuperClass() {
	spritePtr = new Sprite( Sprite::TYPE_DEFAULT, "materials/engine/default.png" );
}

ASprite::~ASprite() {
	delete spritePtr;
}

void ASprite::SetSpriteTexture( const std::string &path ) {
	spritePtr->SetTexture( path );
}

void ASprite::Tick() {
	SuperClass::Tick();

	SetAngles( angles_.GetValue() + 0.2f );

	spritePtr->SetPosition( position_ );
	spritePtr->SetAngles( angles_ );
}

void ASprite::Draw() {
	SuperClass::Draw();

	spritePtr->Draw();
}

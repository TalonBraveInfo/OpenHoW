// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"
#include "Sprite.h"
#include "ActorManager.h"

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
	ohw::Sprite *spritePtr;
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

	SetAngles( myAngles.GetValue() + 0.2f );

	spritePtr->SetPosition( position_ );
	spritePtr->SetAngles( myAngles );
}

void ASprite::Draw() {
	SuperClass::Draw();

	spritePtr->Draw();
}

/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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

#include "../../engine.h"
#include "../../input.h"
#include "../../Map.h"

#include "../actor_manager.h"
#include "../player.h"

#include "actor.h"

using namespace openhow;

Actor::Actor() :
	INIT_PROPERTY( input_forward, PROP_PUSH, 0.00 ),
	INIT_PROPERTY( input_yaw, PROP_PUSH, 0.00 ),
	INIT_PROPERTY( input_pitch, PROP_PUSH, 0.00 ),
	INIT_PROPERTY( position_, PROP_LOCAL | PROP_WRITE, PLVector3( 0, 0, 0 ) ),
	INIT_PROPERTY( fallback_position_, PROP_LOCAL | PROP_WRITE, PLVector3( 0, 0, 0 ) ),
	INIT_PROPERTY( angles_, PROP_LOCAL | PROP_WRITE, PLVector3( 0, 0, 0 ) ),
	INIT_PROPERTY( bounds_, PROP_LOCAL | PROP_WRITE, PLVector3( 0, 0, 0 ) ) {}

Actor::~Actor() {
	for ( auto actor : children_ ) {
		ActorManager::GetInstance()->DestroyActor( actor );
		actor = nullptr;
	}

	children_.clear();
	children_.shrink_to_fit();

	DestroyPhysicsBody();
}

void Actor::SetAngles( PLVector3 angles ) {
	VecAngleClamp( &angles );
	old_angles_ = angles_;
	angles_ = angles;
}

void Actor::SetPosition( PLVector3 position ) {
	old_position_ = position_;
	position_ = position;
}

void Actor::Deserialize( const ActorSpawn &spawn ) {
	SetPosition( spawn.position );
	SetAngles( spawn.angles );
}

const IPhysicsBody *Actor::CreatePhysicsBody() {
	if ( physics_body_ != nullptr ) {
		return physics_body_;
	}

	physics_body_ = Engine::Physics()->CreatePhysicsBody();
	if ( physics_body_ == nullptr ) {
		return nullptr;
	}

	return physics_body_;
}

void Actor::DestroyPhysicsBody() {
	if ( physics_body_ == nullptr ) {
		return;
	}

	Engine::Physics()->DestroyPhysicsBody( physics_body_ );
	physics_body_ = nullptr;
}

/**
 * Used for inflicting damage upon the actor.
 * @param attacker The actor inflicting the damage.
 * @param damageInflicted The amout of health to subtract.
 * @param direction The direction of the damage.
 * @param velocity Velocity of the damage, for physics.
 * @return Returns true if the actor is killed.
 */
bool Actor::Damage( const Actor *attacker, uint16_t damageInflicted, PLVector3 direction, PLVector3 velocity ) {
	if ( health_ <= 0 ) {
		return true;
	}

	health_ -= damageInflicted;
	if ( health_ <= 0 ) {
		Killed();
		return true;
	}

	return false;
}

void Actor::Killed() {
	ActorManager::GetInstance()->DestroyActor( this );
}

void Actor::AddHealth( int16_t health ) {
	if ( health <= 0 ) {
		return;
	}

	health_ += health;
}

bool Actor::Possessed( const Player *player ) {
	return true;
}

void Actor::Depossessed( const Player *player ) {}

/**
 * Drop the actor to the ground based on it's bounding box size.
 */
void Actor::DropToFloor() {
	Map *map = Engine::Game()->GetCurrentMap();
	if ( map == nullptr ) {
		Error( "Failed to get current map!\n" );
	}

	PLVector3 nPosition = position_;
	float height = map->GetTerrain()->GetHeight( PLVector2( nPosition.x, nPosition.z ) );
	nPosition.y = height + bounds_.GetValue().y;
	SetPosition( nPosition );
}

PLVector3 Actor::GetForward() {
	return plNormalizeVector3( {
								   cosf( plDegreesToRadians( angles_.GetValue().y ) )
									   * cosf( plDegreesToRadians( angles_.GetValue().x ) ),
								   sinf( plDegreesToRadians( angles_.GetValue().x ) ),
								   sinf( plDegreesToRadians( angles_.GetValue().y ) )
									   * cosf( plDegreesToRadians( angles_.GetValue().x ) )
							   } );
}

void Actor::HandleInput() {
	IGameMode *mode = Engine::Game()->GetMode();
	if ( mode == nullptr ) {
		return;
	}

	Player *player = mode->GetCurrentPlayer();
	if ( player == nullptr ) {
		return;
	}

	PLVector2 cl = Input_GetJoystickState( player->GetControllerSlot(), INPUT_JOYSTICK_LEFT );
	PLVector2 cr = Input_GetJoystickState( player->GetControllerSlot(), INPUT_JOYSTICK_RIGHT );

	if ( Input_GetActionState( player->GetControllerSlot(), ACTION_MOVE_FORWARD ) ) {
		input_forward = 1.0f;
	} else if ( Input_GetActionState( player->GetControllerSlot(), ACTION_MOVE_BACKWARD ) ) {
		input_forward = -1.0f;
	} else {
		input_forward = -cl.y / 327.0f;
	}

	if ( Input_GetActionState( player->GetControllerSlot(), ACTION_TURN_LEFT ) ) {
		input_yaw = -1.0f;
	} else if ( Input_GetActionState( player->GetControllerSlot(), ACTION_TURN_RIGHT ) ) {
		input_yaw = 1.0f;
	} else {
		input_yaw = cr.x / 327.0f;
	}

	if ( Input_GetActionState( player->GetControllerSlot(), ACTION_AIM_UP ) ) {
		input_pitch = 1.0f;
	} else if ( Input_GetActionState( player->GetControllerSlot(), ACTION_AIM_DOWN ) ) {
		input_pitch = -1.0f;
	} else {
		input_pitch = -cr.y / 327.0f;
	}
}

/**
 * Attach actor to self, and set self as parent. Children are automatically destroyed.
 * @param actor Child actor to link.
 */
void Actor::LinkChild( Actor *actor ) {
	if ( actor == nullptr ) {
		LogWarn( "Attempted to attach an invalid actor to self!\n" );
		return;
	}

	children_.push_back( actor );
	actor->parent_ = this;
}

/**
 * Called when one actor collides with another.
 * @param other The touchee.
 */
void Actor::Touch( Actor *other ) {
	LogDebug( "actor (%s) touched actor (%s)\n",
			  plPrintVector3( &position_.GetValue(), pl_int_var ),
			  plPrintVector3( &other->position_.GetValue(), pl_int_var ) );
}

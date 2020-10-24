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
#include "Map.h"
#include "ActorManager.h"
#include "Player.h"
#include "Actor.h"

#include "graphics/Camera.h"

using namespace ohw;

Actor::Actor() :
	INIT_PROPERTY( forwardVelocity, PROP_PUSH, 0.00 ),
	INIT_PROPERTY( inputYaw, PROP_PUSH, 0.00 ),
	INIT_PROPERTY( inputPitch, PROP_PUSH, 0.00 ),
	INIT_PROPERTY( position_, PROP_LOCAL | PROP_WRITE, PLVector3( 0, 0, 0 ) ),
	INIT_PROPERTY( fallback_position_, PROP_LOCAL | PROP_WRITE, PLVector3( 0, 0, 0 ) ),
	INIT_PROPERTY( angles_, PROP_LOCAL | PROP_WRITE, PLVector3( 0, 0, 0 ) ) {}

Actor::~Actor() {
	for ( auto actor : childActors ) {
		ActorManager::GetInstance()->DestroyActor( actor );
		actor = nullptr;
	}

	childActors.clear();
	childActors.shrink_to_fit();

	DestroyPhysicsBody();
}

/**
 * Simulation tick, called per-frame.
 */
void Actor::Tick() {}

void Actor::SetAngles( PLVector3 angles ) {
	VecAngleClamp( &angles );
	old_angles_ = angles_;
	angles_ = angles;
}

bool Actor::IsVisible() {
	Camera *camera = Engine::Game()->GetActiveCamera();
	return !( camera == nullptr || !camera->IsBoxVisible( &boundingBox ) );
}

void Actor::SetVelocity(PLVector3 newVelocity) {
	old_velocity_ = velocity;
	velocity = newVelocity;
}

void Actor::SetPosition( PLVector3 position ) {
	old_position_ = position_;
	position_ = position;

	// Ensure the bounds are kept updated...
	boundingBox.origin = position;
}

void Actor::Deserialize( const ActorSpawn &spawn ) {
	// Convert the original spawn bounds to those we want (TODO: do this in spawn handler)
	boundingBox.maxs.x = ( float ) spawn.bounds[ 0 ];
	boundingBox.mins.x = ( float ) -spawn.bounds[ 0 ];
	boundingBox.maxs.y = ( float ) spawn.bounds[ 1 ];
	boundingBox.mins.y = ( float ) -spawn.bounds[ 1 ];
	boundingBox.maxs.z = ( float ) spawn.bounds[ 2 ];
	boundingBox.mins.z = ( float ) -spawn.bounds[ 2 ];

	SetPosition( spawn.position );
	SetAngles( spawn.angles );
}

const ohw::PhysicsBody *Actor::CreatePhysicsBody() {
#if 0
	if ( physics_body_ != nullptr ) {
		return physics_body_;
	}

	physics_body_ = Engine::Physics()->CreatePhysicsBody();
	if ( physics_body_ == nullptr ) {
		return nullptr;
	}

	return physics_body_;
#else
	return nullptr;
#endif
}

void Actor::DestroyPhysicsBody() {
#if 0
	if ( physics_body_ == nullptr ) {
		return;
	}

	Engine::Physics()->DestroyPhysicsBody( physics_body_ );
	physics_body_ = nullptr;
#endif
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

void Actor::Dispossessed(const Player *player ) {
	// Clear all the input, otherwise we'll just run off forever
	forwardVelocity = inputPitch = inputYaw = 0.0f;
}

/**
 * Drop the actor to the ground based on it's bounding box size.
 */
void Actor::DropToFloor() {
	Map *map = Engine::Game()->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	Terrain *terrain = map->GetTerrain();
	if ( terrain == nullptr ) {
		return;
	}

	// Fetch the height based on where we're currently positioned
	PLVector3 newPos = position_;
	float height = terrain->GetHeight( PLVector2( newPos.x, newPos.z ) );

	// Now add the bounding box height onto that, so we're raised a little off the surface
	newPos.y = height + ( boundingBox.maxs.y / 2 );
	
	// And finally actually update our position
	SetPosition( newPos );
}

PLVector3 Actor::GetForward() {
	float x = cosf( plDegreesToRadians( angles_.GetValue().y ) ) * cosf( plDegreesToRadians( angles_.GetValue().x ) );
	float y = sinf( plDegreesToRadians( angles_.GetValue().x ) );
	float z = sinf( plDegreesToRadians( angles_.GetValue().y ) ) * cosf( plDegreesToRadians( angles_.GetValue().x ) );
	return plNormalizeVector3( PLVector3( x, y, z ) );
}

/**
 * Attach actor to self, and set self as parent. Children are automatically destroyed.
 * @param actor Child actor to link.
 */
void Actor::LinkChild( Actor *actor ) {
	if ( actor == nullptr ) {
		Warning( "Attempted to attach an invalid actor to self!\n" );
		return;
	}

	childActors.push_back( actor );
	actor->parentActor = this;
}

/**
 * Checks whether or not this actor is touching another actor.
 */
bool Actor::CheckTouching() {
	bool touchedSomething = false;

	ActorSet actorSet = ActorManager::GetInstance()->GetActors();
	for ( Actor *other : actorSet ) {
		// Can't touch ourself
		if ( other == this || !other->IsActivated() ) {
			continue;
		}

		// Check if it's our parent; we can't touch them
		if ( other == parentActor ) {
			continue;
		}

		// Check if it's one of our children; we can't touch them either
		bool isChild = false;
		for ( unsigned int i = 0; i < childActors.size(); ++i ) {
			if ( childActors[ i ] == other ) {
				isChild = true;
				break;
			}
		}

		if ( isChild ) {
			continue;
		}

		// Now check the AABB against that of the other actor
		if( !plIsAABBIntersecting( &boundingBox, &other->boundingBox ) ) {
			// Didn't get a hit, continue onto the next
			continue;
		}

		// We're touching them, so go ahead and handle that case
		Touch( other );

		touchedSomething = true;
	}

	return touchedSomething;
}

/**
 * Called when one actor collides with another.
 * @param other The touchee.
 */
void Actor::Touch( Actor *other ) {
#if 0
	LogDebug( "actor %s (%s) touched actor %s (%s)\n",
		GetClassName(),
		plPrintVector3( &position_.GetValue(), pl_int_var ),
		GetClassName(),
		plPrintVector3( &other->position_.GetValue(), pl_int_var ) );
#endif
}

/**
 * Check whether or not this actor is currently on the ground.
 */
bool Actor::IsGrounded() {
	Map *map = Engine::Game()->GetCurrentMap();
	if ( map == nullptr ) {
		return false;
	}

	PLVector3 nPosition = position_;
	float tileHeight = map->GetTerrain()->GetHeight( PLVector2( nPosition.x, nPosition.z ) );
	return ( nPosition.y - ( boundingBox.maxs.y / 2 ) ) <= tileHeight;
}

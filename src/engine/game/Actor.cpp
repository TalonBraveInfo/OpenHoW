// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

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
	INIT_PROPERTY( position_, PROP_LOCAL | PROP_WRITE, hei::Vector3( 0, 0, 0 ) ),
	INIT_PROPERTY( fallback_position_, PROP_LOCAL | PROP_WRITE, hei::Vector3( 0, 0, 0 ) ),
	INIT_PROPERTY( myAngles, PROP_LOCAL | PROP_WRITE, hei::Vector3( 0, 0, 0 ) ) {}

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
void Actor::Tick() {
	myForward = CalculateForwardVector();
}

void Actor::SetAngles( hei::Vector3 angles ) {
	VecAngleClamp( &angles );
	myOldAngles = myAngles;
	myAngles = angles;
}

bool Actor::IsVisible() {
	Camera *camera = GetApp()->gameManager->GetActiveCamera();
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
	return nullptr;
}

void Actor::DestroyPhysicsBody() {}

/**
 * Used for inflicting damage upon the actor.
 * @param attacker The actor inflicting the damage.
 * @param damageInflicted The amout of health to subtract.
 * @param direction The direction of the damage.
 * @param velocity Velocity of the damage, for physics.
 * @return Returns true if the actor is killed.
 */
bool Actor::Damage( const Actor *attacker, uint16_t damageInflicted, PLVector3 direction, PLVector3 velocity ) {
	if ( myHealth <= 0 ) {
		return true;
	}

	myHealth -= damageInflicted;
	if ( myHealth <= 0 ) {
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

	myHealth += health;
}

bool Actor::Possessed( const Player *player ) {
	if ( myHealth <= 0 ) {
		return false;
	}

	return true;
}

void Actor::Dispossessed(const Player *player ) {
	ClearInput();
}

/**
 * Drop the actor to the ground based on it's bounding box size.
 */
void Actor::DropToFloor() {
	Map *map = GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		return;
	}

	Terrain *terrain = map->GetTerrain();
	if ( terrain == nullptr ) {
		return;
	}

	// Fetch each of the tiles we're possibly touching.
	const Terrain::Tile *tiles[ 4 ];
	tiles[ 0 ] = terrain->GetTile( boundingBox.mins.x + position_.GetValue().x, boundingBox.mins.z + position_.GetValue().z );
	tiles[ 1 ] = terrain->GetTile( boundingBox.maxs.x + position_.GetValue().x, boundingBox.maxs.z + position_.GetValue().z );
	tiles[ 2 ] = terrain->GetTile( boundingBox.mins.x + position_.GetValue().x, boundingBox.maxs.z + position_.GetValue().z );
	tiles[ 3 ] = terrain->GetTile( boundingBox.mins.x + position_.GetValue().x, boundingBox.maxs.z + position_.GetValue().z );

	// Iterate through to figure out the height we should be at.
	float maxTileHeight = terrain->GetMinHeight();
	unsigned int tileNum = 0;
	for ( ; tileNum < 4; ++tileNum ) {
		for ( unsigned int corner = 0; corner < 4; ++corner ) {
			if ( tiles[ tileNum ]->height[ corner ] <= maxTileHeight ) {
				continue;
			}

			maxTileHeight = tiles[ tileNum ]->height[ corner ];
		}
	}
	
	// And finally actually update our position
	//SetPosition( newPos );
}

/**
 * Get forward vector based on current angles.
 */
PLVector3 Actor::CalculateForwardVector() {
	float rx = PlDegreesToRadians( myAngles.GetValue().x );
	float ry = PlDegreesToRadians( myAngles.GetValue().y );

	PLVector3 v;
	v.x = std::cos( ry ) * std::cos( rx );
	v.y = std::sin( rx );
	v.z = std::sin( ry ) * std::cos( rx );

	return PlNormalizeVector3( v );
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
		if( !PlIsAabbIntersecting( &boundingBox, &other->boundingBox ) ) {
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
bool Actor::IsOnGround() {
	Map *map = GetApp()->gameManager->GetCurrentMap();
	if ( map == nullptr ) {
		return false;
	}

	PLVector3 nPosition = position_;
	float tileHeight = map->GetTerrain()->GetHeight( nPosition.x, nPosition.z );
	return ( nPosition.y - ( boundingBox.maxs.y / 2 ) ) <= tileHeight;
}

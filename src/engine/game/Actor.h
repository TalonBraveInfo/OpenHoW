// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include "../Property.h"

enum ActorFlag {
	ACTOR_FLAG_PLAYABLE = 1,
	ACTOR_FLAG_SCRIPTED = 16,
	ACTOR_FLAG_INSIDE = 32,
	ACTOR_FLAG_DELAYED = 64,
	/* ...any new types below... */
};

enum ActorEvent {
	ACTOR_EVENT_NONE,                       // does nothing!
	ACTOR_EVENT_ITEM,                       // spawns an item on destruction
	ACTOR_EVENT_PROMOTION,                  // spawns a promotion point on destruction
	ACTOR_EVENT_PROTECT = 4,                // spawns a promotion point if the object is not destroyed
	ACTOR_EVENT_AIRDROP_ITEM = 7,           // spawns item airdrop on destruction
	ACTOR_EVENT_AIRDROP_PROMOTION,          // spawns promotion point on destruction
	ACTOR_EVENT_GROUP_ITEM = 13,            // spawns item when group is destroyed
	ACTOR_EVENT_GROUP_PROMOTION,            // spawns promotion point when group is destroyed
	ACTOR_EVENT_REWARD = 19,                // returns specified item on destruction to destructor
	ACTOR_EVENT_GROUP_SPAWN,                // spawns group on destruction
	ACTOR_EVENT_VICTORY,                    // triggers victory on destruction
	ACTOR_EVENT_BURST,                      // spawns group upon destruction by TNT
	ACTOR_EVENT_GROUP_OBJECT,               // spawns group when object's group is destroyed
	/* ...any new types below... */
};

struct ActorSpawn {
	std::string className;
	PLVector3 position; // position in the world
	uint16_t index;                  // todo
	PLVector3 angles; // angles in the world
	uint16_t type;                   // todo

	int16_t bounds[3]; // collision bounds
	uint16_t bounds_type; // box, prism, sphere and none

	int16_t energy;
	uint8_t appearance;
	uint8_t team; // uk, usa, german, french, japanese, soviet

	uint16_t objective;
	uint8_t objective_actor_id;
	uint8_t objective_extra[2];

	PLVector3 fallbackPosition;

	int16_t extra;
	ActorSpawn *attachment{ nullptr };
};

namespace ohw {
	class PhysicsBody;
}

#define IMPLEMENT_SUPER( a ) typedef a SuperClass;
#define IMPLEMENT_ACTOR( base, parent ) \
  IMPLEMENT_SUPER(parent) \
  public: inline const char* GetClassName() override { return PL_STRINGIFY(base) ; } \
  private:

class Player;

class Actor : public PropertyOwner {
public:
	Actor();
	~Actor() override;

	virtual const char *GetClassName() { return "Actor"; }

	virtual void Tick();
	virtual void Draw() {}  // draw tick, called per-frame

	virtual bool Damage( const Actor *attacker,
	                     uint16_t damageInflicted,
	                     PLVector3 direction = { 0.0f, 0.0f, 0.0f },
	                     PLVector3 velocity = { 0.0f, 0.0f, 0.0f } );
	virtual void Killed();

	virtual void SetHealth( int16_t health ) { myHealth = health; }
	virtual void AddHealth( int16_t health );
	int16_t GetHealth() { return myHealth; }

	virtual bool IsVisible();

	inline PLVector3 GetVelocity() const {
		return velocity;
	}
	virtual void SetVelocity( PLVector3 newVelocity );

	inline PLVector3 GetPosition() const {
		return position_;
	}
	virtual void SetPosition( PLVector3 position );

	inline PLVector3 GetAngles() const {
		return myAngles;
	}
	virtual void SetAngles( hei::Vector3 angles );

	inline PLVector3 GetForward() const {
		return myForward;
	}

	inline float GetHeight() const {
		return position_.GetValue().y;
	}

	virtual bool Possessed( const Player *player );
	virtual void Dispossessed( const Player *player );
	virtual void HandleInput() {}   // handle any player input, if applicable

	virtual ActorSpawn Serialize() { return ActorSpawn(); }
	virtual void Deserialize( const ActorSpawn &spawn );

	virtual void Activate() { isActive = true; }
	virtual void Deactivate() { isActive = false; }
	virtual bool IsActivated() { return isActive; }

	virtual bool IsVisibleOnMinimap() const { return false; }
	virtual unsigned int GetMinimapIconStyle() const { return 0; }
	virtual PLColour GetMinimapIconColour() const { return { 255, 255, 255, 255 }; }

	Actor *GetParent() { return parentActor; }
	void LinkChild( Actor *actor );
	unsigned int GetNumOfChildren() { return childActors.size(); }
	std::vector< Actor * > GetChildren() { return childActors; }

	bool CheckTouching();
	virtual void Touch( Actor *other );

	void DropToFloor();

	// Physics

	const ohw::PhysicsBody *CreatePhysicsBody();
	void DestroyPhysicsBody();

	PLCollisionAABB *GetBoundingBox() { return &boundingBox; }

	// Networking

	void SendUpdate();
	void ReceiveUpdate();

protected:
	PLVector3 CalculateForwardVector();

	bool IsOnGround();

	bool is_visible_{ false };

	NumericProperty< float > forwardVelocity;  /* -1.0 = backwards, +1.0 = forwards */
	NumericProperty< float > inputYaw;      /* -1.0 = left, +1.0 = right */
	NumericProperty< float > inputPitch;    /* -1.0 = down, +1.0 = up */

	hei::Vector3 velocity{ 0, 0, 0 }, old_velocity_{ 0, 0, 0 };

	Vector3Property position_;
	hei::Vector3 old_position_{ 0, 0, 0 };

	Vector3Property fallback_position_;

	Vector3Property myAngles;
	hei::Vector3 myOldAngles{ 0.0f, 0.0f, 0.0f };
	hei::Vector3 myForward{ 0.0f, 0.0f, 0.0f };

	PLCollisionAABB boundingBox;

private:
	inline void ClearInput() {
		forwardVelocity = inputPitch = inputYaw = 0.0f;
	}

	int16_t myHealth{ 0 };

	ohw::PhysicsBody *myPhysicsBody{ nullptr };

	bool isActive{ false };

	Actor *parentActor{ nullptr };
	std::vector< Actor * > childActors;
};

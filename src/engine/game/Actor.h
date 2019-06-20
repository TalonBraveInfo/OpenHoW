/* OpenHoW
 * Copyright (C) 2017-2019 Mark Sowden <markelswo@gmail.com>
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

#pragma once

#include <PL/platform_math.h>
#include <map>
#include <string>

#include "msg.h"

class Actor;

enum ActorFlag {
    /* ...original... */
    ACTOR_FLAG_PLAYABLE = 1,
    ACTOR_FLAG_SCRIPTED = 16,
    ACTOR_FLAG_INSIDE   = 32,
    ACTOR_FLAG_DELAYED  = 64,
    /* ...any new types below... */
};

enum ActorEvent {
    /* ...original... */
    ACTOR_EVENT_NONE,                       // does nothing!
    ACTOR_EVENT_ITEM,                       // spawns an item on destruction
    ACTOR_EVENT_PROMOTION,                  // spawns a promotion point on destruction
    ACTOR_EVENT_PROTECT             = 4,    // spawns a promotion point if the object is not destroyed
    ACTOR_EVENT_AIRDROP_ITEM        = 7,    // spawns item airdrop on destruction
    ACTOR_EVENT_AIRDROP_PROMOTION,          // spawns promotion point on destruction
    ACTOR_EVENT_GROUP_ITEM          = 13,   // spawns item when group is destroyed
    ACTOR_EVENT_GROUP_PROMOTION,            // spawns promotion point when group is destroyed
    ACTOR_EVENT_REWARD              = 19,   // returns specified item on destruction to destructor
    ACTOR_EVENT_GROUP_SPAWN,                // spawns group on destruction
    ACTOR_EVENT_VICTORY,                    // triggers victory on destruction
    ACTOR_EVENT_BURST,                      // spawns group upon destruction by TNT
    ACTOR_EVENT_GROUP_OBJECT,               // spawns group when object's group is destroyed
    /* ...any new types below... */
};

/* An ActorProperty object represents a "property" of an Actor instance.
 * Properties are member variables within the Actor which can be enumerated at runtime for access
 * from scripts, debugging, and also form the base of the networking mechanism.
 *
 * Properties are used for anything which must be synchronised between network nodes such as an\
 * actor's position and speed, but also input which drive the simulation.
 *
 * For example, an actor might be able to move an actor by pressing a button. While the player has
 * that button pressed, the active player's client will set an input property on that actor which
 * instructs the actor to move forward. On each tick of the simulation, that actor will check for
 * the "moving forward" property, and, if set, update the actor's position/speed/etc accordingly.
 *
 * All instances of the game run their own independant simulation, any small discrepencies in
 * position/etc due to (for example) floating point differences between the systems will be fixed
 * up by a periodic synchronisation from the server, which pushes out updates for properties
 * changed by its own simulation.
 *
 * Each instance of an ActorProperty maintains a dirty/clean state, the simulation tick where it
 * became dirty (if applicable), its current value, default value and the last "clean" value. In a
 * multiplayer game, this is used for maintaining synchronisation between hosts.
 *
 * This is slightly ugly since every ActorProperty class has to duplicate some of the above, but
 * the alternative was making the multiplayer/networking code maintain its own lists of ALL
 * properties, their clean values and dirty ticks, which would be more complicated and likely to
 * develop bugs.
*/

class ActorProperty
{
public:
    /* Flags can be bitwise ORd together. */
    enum Flags {
        /* No special handling. */
        DEFAULT = 0,

        /* Changes made to this property during input handling or simulation will immediately be
         * queued to other nodes after the phase completes. Example use cases are inputs and
         * flip-flop properties set by the simulation which do something next tick, then reset
         * themselves, to ensure each node has a tick where the flip is set.
        */
        IMMEDIATE = (1 << 0),

        /* This property is an input, can be set by the active player and is automatically reset
         * to its default state by the server at the end of a turn.
        */
        INPUT = (1 << 1) | IMMEDIATE,

        /* I can't remember what I was going to use this for... something which is only set
         * server-side... but what?
        */
        OUTPUT = (1 << 2),
    };

    const std::string name;
    const Flags flags;

private:
    Actor &actor;

    bool dirty;
    unsigned int dirty_tick;

protected:
    ActorProperty(Actor &actor, const std::string &name, Flags flags);

    /* Mark the property dirty.
     * Classes MUST call this on EVERY change to their value.
    */
    void mark_dirty();

    /* Mark the property clean. */
    void mark_clean();

public:
    virtual ~ActorProperty();

    /* Revert to default value. Makes DIRTY. */
    virtual void reset() = 0;

    /* Make current value the clean value. Makes CLEAN. */
    virtual void commit() = 0;

    /* Rollback to the last clean value. Makes DIRTY. */
    virtual void rollback() = 0;

    virtual void to_msg(NetMessage *msg) const = 0;
    virtual void from_msg(const NetMessage *msg) = 0;

    bool is_dirty() const;            /* Is the property presently dirty? */
    unsigned int dirty_ticks() const; /* Number of ticks the property has been dirty for. */
};

class Vector3ActorProperty: public ActorProperty
{
private:
    class FloatProxy
    {
    private:
        Vector3ActorProperty &property;
        float &value;

    public:
        FloatProxy(Vector3ActorProperty &property, float &value):
            property(property), value(value) {}

        operator float() const
        {
            return value;
        }

        FloatProxy &operator=(const float &rhs)
        {
            value = rhs;
            property.mark_dirty();

            return *this;
        }

        FloatProxy &operator+=(const float &rhs)
        {
            value += rhs;
            property.mark_dirty();

            return *this;
        }

        FloatProxy &operator-=(const float &rhs)
        {
            value -= rhs;
            property.mark_dirty();

            return *this;
        }
    };

    const PLVector3 default_value;
    PLVector3 clean_value, value;

public:
    FloatProxy x, y, z;

    Vector3ActorProperty(Actor &actor, const std::string &name, Flags flags, const PLVector3 &default_value = {0, 0, 0}):
        ActorProperty(actor, name, flags),
        default_value(default_value),
        clean_value(default_value),
        value(default_value),
        x(*this, value.x),
        y(*this, value.y),
        z(*this, value.z) {}

    operator const PLVector3&() const
    {
        return value;
    }

    Vector3ActorProperty &operator=(const PLVector3 &rhs)
    {
        value = rhs;
        mark_dirty();

        return *this;
    }

    virtual void reset() override
    {
        value = default_value;
        mark_dirty();
    }

    virtual void commit() override
    {
        clean_value = value;
        mark_clean();
    }

    virtual void rollback() override
    {
        value = clean_value;
        mark_clean();
    }

    virtual void to_msg(NetMessage *msg) const override
    {
        float *data = (float*)(msg->property_value);
        data[0] = value.x;
        data[1] = value.y;
        data[2] = value.z;
    }
    
    virtual void from_msg(const NetMessage *msg) override
    {
        float *data = (float*)(msg->property_value);
        value.x = data[0];
        value.y = data[1];
        value.z = data[2];
    }
};

template<typename T> class PODActorProperty: public ActorProperty
{
private:
    const T default_value;
    T clean_value, value;

public:
    PODActorProperty(Actor &actor, const std::string &name, Flags flags, const T &default_value):
        ActorProperty(actor, name, flags),
        default_value(default_value),
        clean_value(default_value),
        value(default_value) {}

    operator const T&() const
    {
        return value;
    }

    PODActorProperty &operator=(const T &rhs)
    {
        if(rhs == value)
        {
            return *this;
        }

        value = rhs;
        mark_dirty();

        return *this;
    }

    virtual void reset() override
    {
        value = default_value;
        mark_dirty();
    }

    virtual void commit() override
    {
        clean_value = value;
        mark_clean();
    }

    virtual void rollback() override
    {
        value = clean_value;
        mark_clean();
    }

    virtual void to_msg(NetMessage *msg) const override
    {
        memcpy(msg->property_value, &value, sizeof(value));
    }

    virtual void from_msg(const NetMessage *msg) override
    {
        memcpy(&value, msg->property_value, sizeof(value));
    }
};

typedef PODActorProperty<bool> BooleanActorProperty;

class Actor {
friend ActorProperty;
public:
    std::map<std::string, ActorProperty*> properties_;

public:

    Actor();
    explicit Actor(const std::string& name);
    virtual ~Actor();

    virtual void Tick();    // simulation tick, called per-frame
    virtual void Draw() {}  // draw tick, called per-frame

    //virtual bool Possess(Player* player);
    //virtual bool Depossess(Player* player);

    virtual bool IsVisible() { return is_visible_; }

    virtual PLVector3 GetPosition() { return position_; }
    virtual void SetPosition(PLVector3 position) { position_ = position; }

    virtual PLVector3 GetAngles() { return angles_; }
    virtual void SetAngles(PLVector3 angles);

    virtual void HandleInput();   // handle any player input, if applicable

protected:
    bool is_visible_{false};

    Vector3ActorProperty position_;           // x, y, z
    PLVector3 fallback_position_{0, 0, 0};
    PLVector3 angles_{0, 0, 0};             // p, y, r
    PLVector3 bounds_{0, 0, 0};

    BooleanActorProperty move_forward, move_backward, move_up, move_down, turn_left, turn_right;

    std::string spawn_name{"none"};

private:
    uint16_t flags_{0};

    uint16_t    team_{0};
    int16_t     health_{0};

    Actor* parent_{nullptr};
    Actor* child_{nullptr};

    /* todo: collision sys */
};

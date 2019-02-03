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

#include "../pork.h"
#include "../pork_game.h"
#include "../pork_player.h"

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

class Actor {
public:
    Actor();
    Actor(PLVector3 position);
    virtual ~Actor();

    virtual void Simulate() {}
    virtual void Draw() {}

    virtual bool Possess(Player *player);
    virtual bool Depossess(Player *player);

    virtual bool IsVisible() { return is_visible_; }

    virtual PLVector3 GetPosition() { return position_; }
    virtual void SetPosition(PLVector3 position) { position_ = position; }

    Actor *parent_{nullptr};
    Actor *child_{nullptr};

protected:
private:
    uint16_t flags_{0};

    bool is_visible_{false};

    PLVector3 position_{0, 0, 0};
    PLVector3 angles_{0, 0, 0};
    PLVector3 bounds_{0, 0, 0};

    Player *controller_{nullptr};

    /* todo: collision sys */
};

class ActorManager {
protected:
    typedef Actor *(*actor_ctor_func)();
    static std::map<std::string, actor_ctor_func> actor_classes_;

public:
    static Actor *SpawnActor(const std::string &name);
    static void DestroyActor(Actor *actor);

    static void SimulateActors();
    static void DrawActors();

    class ActorClassRegistration {
    public:
        const std::string name_;

        ActorClassRegistration(const std::string &name, actor_ctor_func ctor_func);
        ~ActorClassRegistration();
    };

private:
    static std::vector<Actor*> actors_;
};

#define register_actor(NAME, CLASS) \
    static Actor * CLASS ## _make() { return new CLASS (); } \
    static ActorManager::ActorClassRegistration _reg_actor_name((NAME), CLASS ## _make) __attribute__ ((init_priority(2000)))

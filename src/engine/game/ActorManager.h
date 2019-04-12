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

#include "../util.h"

class Actor;

class ActorManager {
protected:
    typedef Actor *(*actor_ctor_func)();
    static std::map<std::string, actor_ctor_func> actor_classes_;

public:
    static ActorManager *GetInstance() {
        static ActorManager *instance = nullptr;
        if(instance == nullptr) {
            instance = new ActorManager();
        }
        return instance;
    }

    Actor *SpawnActor(const std::string &name);
    void DestroyActor(Actor *actor);

    void SimulateActors();
    void DrawActors();
    void ClearActors();

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

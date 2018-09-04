/* OpenHoW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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

#include <pork_engine.h>
#include "../pork_engine.h"

#include "actor.h"

/* Actor Factory */

std::map<std::string, ActorFactory::actor_ctor_func> ActorFactory::actor_classes_
    __attribute__((init_priority (1000)));

Actor *ActorFactory::CreateActor(const std::string &name) {
    auto i = actor_classes_.find(name);
    if(i == actor_classes_.end()) {
        abort();
    }
    return i->second();
}

ActorFactory::ActorClassRegistration::ActorClassRegistration(const std::string &name, actor_ctor_func ctor_func)
    : name_(name) {
    ActorFactory::actor_classes_[name] = ctor_func;
}

ActorFactory::ActorClassRegistration::~ActorClassRegistration() {
    ActorFactory::actor_classes_.erase(name_);
}

/* Actor */

Actor::Actor(PLVector3 position) {

}

Actor::~Actor() {

}

bool Actor::Possess(Player *player) {
    if(!player->is_active) {
        LogWarn("failed to possess actor, inactive player!\n");
        return false;
    }

    if(controller_ != nullptr && controller_ == player) {
        return true;
    } else if(controller_ != nullptr && controller_ != player) {
        if(!Depossess(controller_)) {
            LogWarn("failed to possess actor, already occupied!\n");
            return false;
        }
    }

    controller_ = player;
    return true;
}

bool Actor::Depossess(Player *player) {
    if(controller_ != nullptr && controller_ != player) {
        return false;
    }

    controller_ = nullptr;
    return true;
}

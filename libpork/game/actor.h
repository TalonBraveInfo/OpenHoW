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

#pragma once

class Actor {
public:
    Actor(PLVector3 position);
    ~Actor();

    virtual void Update() {}

    virtual bool Possess(Player *player);
    virtual bool Depossess(Player *player);

protected:
private:
    uint16_t flags_{0};

    PLVector3 position_{0, 0, 0};
    PLVector3 angles_{0, 0, 0};
    PLVector3 bounds_{0, 0, 0};

    Player *controller_{nullptr};

    /* todo: collision sys */
};

class ActorFactory {
protected:
    typedef Actor *(*actor_ctor_func)();
    static std::map<std::string, actor_ctor_func> actor_classes_;

public:
    static Actor *CreateActor(const std::string &name);

    class ActorClassRegistration {
    public:
        const std::string name_;

        ActorClassRegistration(const std::string &name, actor_ctor_func ctor_func);
        ~ActorClassRegistration();
    };
};

#define register_actor(CLASS) \
    static Actor * CLASS ## _make() { return new CLASS (); } \
    static ActorFactory::ActorClassRegistration _reg_actor_name(#CLASS & CLASS ## _make) __attribute__ ((init_priority(2000)))

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

#include "../Map.h"

#include "actors/actor.h"

class ActorManager {
 protected:
  typedef Actor* (* actor_ctor_func_spawn)(const struct ActorSpawn& spawn);
  typedef Actor* (* actor_ctor_func)();
  struct GeneratorFunc {
    actor_ctor_func ctor;
    actor_ctor_func_spawn ctor_spawn;
  };
  static std::map<std::string, GeneratorFunc> actor_classes_;

 public:
  static ActorManager* GetInstance() {
    static ActorManager* instance = nullptr;
    if (instance == nullptr) {
      instance = new ActorManager();
    }
    return instance;
  }

  Actor* CreateActor(const struct ActorSpawn& spawn);
  Actor* CreateActor(const std::string& class_name);
  void DestroyActor(Actor* actor);

  void TickActors();
  void DrawActors();
  void DestroyActors();

  class ActorClassRegistration {
   public:
    const std::string name_;

    ActorClassRegistration(std::string name, actor_ctor_func ctor_func, actor_ctor_func_spawn ctor_func_spawn);
    ~ActorClassRegistration();
  };

 private:
  static std::vector<Actor*> actors_;
};

#define register_actor(NAME, CLASS) \
    static Actor * NAME ## _make_spawn(const struct ActorSpawn& spawn) \
    { return new CLASS (spawn); } \
    static Actor * NAME ## _make() \
    { return new CLASS (); } \
    static ActorManager::ActorClassRegistration __attribute__ ((init_priority(2000))) _reg_actor_ ## NAME ## _name((#NAME), NAME ## _make, NAME ## _make_spawn)

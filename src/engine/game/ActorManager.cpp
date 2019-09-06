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

#include "../engine.h"
#include "../frontend.h"

#include "ActorManager.h"

#include <utility>

#include "actors/actor.h"
#include "actors/actor_static_model.h"

/************************************************************/

std::vector<Actor*> ActorManager::actors_;
std::map<std::string, ActorManager::GeneratorFunc> ActorManager::actor_classes_
    __attribute__((init_priority (1000)));

Actor* ActorManager::CreateActor(const ActorSpawn& spawn) {
  auto i = actor_classes_.find(spawn.class_name);
  Actor* actor;
  if (i == actor_classes_.end()) {
    actor = new AStaticModel(spawn);
  } else {
    actor = i->second.ctor_spawn(spawn);
  }

  actors_.push_back(actor);
  return actor;
}

Actor* ActorManager::CreateActor(const std::string& class_name) {
  auto i = actor_classes_.find(class_name);
  Actor* actor = i->second.ctor();
  actors_.push_back(actor);
  return actor;
}

void ActorManager::DestroyActor(Actor* actor) {
  u_assert(actor != nullptr, "attempted to delete a null actor!\n");
  actors_.erase(std::remove(actors_.begin(), actors_.end(), actor), actors_.end());
  delete actor;
}

void ActorManager::TickActors() {
  for (auto const &actor: actors_) {
    actor->Tick();
  }
}

void ActorManager::DrawActors() {
  if (FrontEnd_GetState() == FE_MODE_LOADING) {
    return;
  }

  g_state.gfx.num_actors_drawn = 0;
  for (auto const &actor: actors_) {
    if (cv_graphics_cull->b_value && !actor->IsVisible()) {
      continue;
    }

    g_state.gfx.num_actors_drawn++;

    actor->Draw();
  }
}

void ActorManager::DestroyActors() {
  for (auto &actor: actors_) {
    delete actor;
  }

  actors_.clear();
}

ActorManager::ActorClassRegistration::ActorClassRegistration(std::string name,
                                                             actor_ctor_func ctor_func,
                                                             actor_ctor_func_spawn ctor_func_spawn)
    : name_(std::move(name)) {
  auto &i = ActorManager::actor_classes_[name];
  i.ctor = ctor_func;
  i.ctor_spawn = ctor_func_spawn;
}

ActorManager::ActorClassRegistration::~ActorClassRegistration() {
  ActorManager::actor_classes_.erase(name_);
}

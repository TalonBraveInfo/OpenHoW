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

#include "actor_manager.h"
#include "actors/actor.h"

/************************************************************/

ActorSet ActorManager::actors_;
std::map<std::string, ActorManager::actor_ctor_func> ActorManager::actor_classes_
    __attribute__((init_priority (1000)));

Actor* ActorManager::CreateActor(const std::string& class_name) {
  auto i = actor_classes_.find(class_name);
  if (i == actor_classes_.end()) {
    // TODO: make this throw an error rather than continue...
    LogWarn("Failed to find actor class %s!\n", class_name.c_str());
    return nullptr;
  }

  Actor* actor = i->second();
  actors_.insert(actor);
  return actor;
}

void ActorManager::DestroyActor(Actor* actor) {
  u_assert(actor != nullptr, "attempted to delete a null actor!\n");
  actors_.erase(actor);
  delete actor;
}

void ActorManager::TickActors() {
  for (auto const& actor: actors_) {
    if(!actor->IsActivated()) {
      continue;
    }

    actor->Tick();
  }
}

void ActorManager::DrawActors() {
  if (FrontEnd_GetState() == FE_MODE_LOADING) {
    return;
  }

  g_state.gfx.num_actors_drawn = 0;
  for (auto const& actor: actors_) {
    if (cv_graphics_cull->b_value && !actor->IsVisible()) {
      continue;
    }

    g_state.gfx.num_actors_drawn++;

    actor->Draw();
  }
}

void ActorManager::DestroyActors() {
  for (auto& actor: actors_) {
    delete actor;
  }

  actors_.clear();
}

void ActorManager::ActivateActors() {
  for(auto const& actor: actors_) {
    actor->Activate();
  }
}

void ActorManager::DeactivateActors() {
  for(auto const& actor: actors_) {
    actor->Deactivate();
  }
}

ActorManager::ActorClassRegistration::ActorClassRegistration(const std::string& name, actor_ctor_func ctor_func)
    : name_(name) {
  ActorManager::actor_classes_[name] = ctor_func;
}

ActorManager::ActorClassRegistration::~ActorClassRegistration() {
  ActorManager::actor_classes_.erase(name_);
}

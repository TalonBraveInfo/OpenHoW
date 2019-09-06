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

class Actor {
 public:

  Actor();
  explicit Actor(const std::string &name);
  virtual ~Actor();

  virtual void Tick() {}  // simulation tick, called per-frame
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

  PLVector3 position_{0, 0, 0};           // x, y, z
  PLVector3 fallback_position_{0, 0, 0};
  PLVector3 angles_{0, 0, 0};             // p, y, r
  PLVector3 bounds_{0, 0, 0};

  std::string spawn_name{"none"};

 private:
  uint16_t flags_{0};

  uint16_t team_{0};
  int16_t health_{0};

  Actor *parent_{nullptr};
  Actor *child_{nullptr};

  /* todo: collision sys */
};

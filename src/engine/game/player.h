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

class Player {
 public:
  explicit Player(Team* team);
  ~Player();

  unsigned int GetNumChildren() { return children_.size(); }
  void AddChild(Actor* actor);
  void RemoveChild(Actor* actor);
  void PossessChild(unsigned int index);
  void DepossessChild();

  void SetControllerSlot(unsigned int slot) { input_slot = slot; }
  unsigned int GetControllerSlot() { return input_slot; }

  const Team* GetTeam() const { return team_; }

 protected:
 private:
  unsigned int input_slot{0}; // Controller slot
  unsigned int identity{}; // Identity of the team (e.g. Tommy etc.)

  Team* team_{nullptr};

  std::vector<Actor*> children_;
  Actor* current_child_{nullptr};
};

/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
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

enum class PlayerType {
  LOCAL,
  NETWORKED,
  COMPUTER,
};

class Player {
 public:
  Player(PlayerType type);
  ~Player();

  unsigned int GetNumChildren() { return children_.size(); }

  void AddChild(Actor *actor);
  void RemoveChild(Actor *actor);

  void PossessCurrentChild();
  void DispossessCurrentChild();

  Actor *GetCurrentChild();

  void CycleChildren(bool forward = true);

  void SetControllerSlot(unsigned int slot) { input_slot = slot; }
  unsigned int GetControllerSlot() { return input_slot; }

  void SetTeam(const PlayerTeam &team) { team_ = team; }
  const PlayerTeam *GetTeam() const { return &team_; }

 protected:
 private:
  unsigned int input_slot{0}; // Controller slot

  PlayerType type_;
  PlayerTeam team_;

  std::vector<Actor *> children_;
  unsigned int currentChildIndex{0};
};

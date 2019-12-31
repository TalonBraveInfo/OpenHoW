/* OpenHoW
 * Copyright (C) 2017-2020 Mark Sowden <markelswo@gmail.com>
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

struct Team;
class Player;

class IGameMode {
 public:
  virtual ~IGameMode() = default;

  virtual void Tick() = 0;

  virtual void StartRound() = 0;
  virtual void RestartRound() = 0;
  virtual void EndRound() = 0;

  virtual void PlayerJoined(Player* player) = 0;
  virtual void PlayerLeft(Player* player) = 0;

  virtual void SpectatorJoined(Player* player) = 0;
  virtual void SpectatorLeft(Player* player) = 0;

  virtual unsigned int GetMaxSpectators() const = 0;
  virtual unsigned int GetMaxPlayers() const = 0;

  virtual Player* GetCurrentPlayer() = 0;

  bool HasModeStarted() const { return mode_started_; }
  bool HasRoundStarted() const { return round_started_; }
  bool HasTurnStarted() const { return turn_started_; }

  unsigned int GetTurnTime() { return num_turn_ticks; }
  unsigned int GetTurnTimeSeconds() { return num_turn_ticks / TICKS_PER_SECOND; }
  unsigned int GetMaxTurnTime() { return max_turn_ticks; }
  unsigned int GetMaxTurnTimeSeconds() { return max_turn_ticks / TICKS_PER_SECOND; }

  virtual void AssignActorToPlayer(Actor* target, Player* owner) = 0;

 protected:
  virtual void StartTurn(Player* player) = 0;
  virtual void EndTurn(Player* player) = 0;

  virtual void SpawnActors() = 0;
  virtual void DestroyActors() = 0;

  unsigned int max_turn_ticks{0}; // Maximum ticks for a turn
  unsigned int num_turn_ticks{0}; // Current number of ticks in the turn

  bool mode_started_{false};
  bool round_started_{false};
  bool turn_started_{false};

  unsigned int current_player_{0 };
};

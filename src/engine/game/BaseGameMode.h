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

class Map;

class BaseGameMode {
public:
    BaseGameMode() = default;
    virtual ~BaseGameMode() ;

    virtual std::string GetDescription() {
        return "singleplayer";
    }

    virtual void StartMode();   // loads necessary map, etc.
    virtual void EndMode();     // ends the mode, cleanup

    virtual uint8_t GetMaxSpectators() {
        return 0;
    }

    virtual uint8_t GetMaxPlayers() {
        return 1;
    }

#if 0
    virtual void SpectatorJoined(uint8_t client_id) = 0;
    virtual void PlayerJoined(uint8_t client_id) = 0;

    virtual void SpectatorLeft(uint8_t client_id) = 0;
    virtual void PlayerLeft(uint8_t client_id) = 0;
#endif

    virtual void Tick();    // called per-frame

    virtual void StartTurn();
    virtual void EndTurn();

    virtual void StartRound(const std::string &map_name);
    virtual void RestartRound();
    virtual void EndRound();

    virtual void SpawnActors();
    virtual void DestroyActors();

    virtual bool HasModeStarted() { return mode_started_; }
    virtual bool HasRoundStarted() { return round_started_; }
    virtual bool HasTurnStarted() { return turn_started_; }

    virtual bool TogglePause() { return (is_paused_ = !is_paused_); }
    virtual bool IsPaused() { return is_paused_; }

protected:
    Map *current_map_{nullptr};

private:
    bool is_paused_{false};

    bool mode_started_{false};
    bool round_started_{false};
    bool turn_started_{false};

    uint8_t turn_time_{99};

    std::vector<uint8_t> players_;      // todo: need proper identifier
    std::vector<uint8_t> spectators_;   // todo: need proper identifier
};

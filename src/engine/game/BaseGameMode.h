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
#include "Game.h"

struct Player;
struct AudioSample;

class Actor;
class AudioSource;

class BaseGameMode {
public:
    BaseGameMode();
    virtual ~BaseGameMode() ;

    virtual std::string GetDescription() {
        return "singleplayer";
    }

    virtual void StartMode(const std::string &map_name);    // loads necessary map, etc.
    virtual void EndMode();                                 // ends the mode, cleanup

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

    virtual void RestartRound();

    virtual bool HasModeStarted() { return mode_started_; }
    virtual bool HasRoundStarted() { return round_started_; }
    virtual bool HasTurnStarted() { return turn_started_; }

    virtual bool TogglePause() { return (is_paused_ = !is_paused_); }
    virtual bool IsPaused() { return is_paused_; }

    Player* GetCurrentPlayer();
    Map* GetCurrentMap() { return current_map_; };

protected:
    Map* current_map_{nullptr};

    virtual void StartTurn();
    virtual void EndTurn();

    virtual void StartRound();
    virtual void EndRound();

    virtual void SpawnActors();
    virtual void DestroyActors();

private:
    bool is_paused_{false};

    bool mode_started_{false};
    bool round_started_{false};
    bool turn_started_{false};

    AudioSource* ambient_emitter_{nullptr};
#define MAX_AMBIENT_SAMPLES 8
    const AudioSample* ambient_samples_[MAX_AMBIENT_SAMPLES]{
            nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
    };

    uint8_t turn_time_{99};

    double ambient_emit_delay_{0};

    std::vector<Player> players_;      // todo: need proper identifier
    unsigned int current_player_{0};
    std::vector<Player> spectators_;   // todo: need proper identifier

    std::vector<Actor*> pigs_;
};

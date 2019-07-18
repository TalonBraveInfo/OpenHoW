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

#include "TempGame.h"
#include "Actor.h"

struct Player {
    std::string     name;
    Actor*          input_target{nullptr};
    unsigned int    input_slot{0};
};

class IGameMode;
class Map;

class GameManager {
private:
    static GameManager* instance_;

public:
    static GameManager* GetInstance() {
        if(instance_ == nullptr) {
            instance_ = new GameManager();
        }

        return instance_;
    }

    static void DestroyInstance() {
        delete instance_;
        instance_ = nullptr;
    }

    /**********************************************/

    GameManager();
    ~GameManager();

    void Tick();

    void LoadMap(const std::string &name);
    void UnloadMap();

    Map* GetCurrentMap() { return active_map_; }

    Player* GetCurrentPlayer();

protected:
private:
    IGameMode*  active_mode_{nullptr};
    Map*        active_map_{nullptr};

#define MAX_AMBIENT_SAMPLES 8
    double                      ambient_emit_delay_{0};
    const struct AudioSample*   ambient_samples_[MAX_AMBIENT_SAMPLES]{};
};

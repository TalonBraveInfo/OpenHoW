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
#include "../audio.h"
#include "../Map.h"

#include "GameManager.h"
#include "SPGameMode.h"

GameManager *GameManager::instance_ = nullptr;

GameManager::GameManager() {

}

GameManager::~GameManager() {

}

void GameManager::Tick() {
    FrontEnd_Tick();

    if(active_mode_ == nullptr) {
        return;
    }

    if(ambient_emit_delay_ < g_state.sim_ticks) {
        const AudioSample* sample = ambient_samples_[rand() % MAX_AMBIENT_SAMPLES];
        if(sample != nullptr) {
            PLVector3 position = {
                    plGenerateRandomf(MAP_PIXEL_WIDTH),
                    active_map_->GetMaxHeight(),
                    plGenerateRandomf(MAP_PIXEL_WIDTH)
            };
            AudioManager::GetInstance()->PlayLocalSound(sample, position, { 0, 0, 0 }, true, 0.5f);
        }

        ambient_emit_delay_ = g_state.sim_ticks + TICKS_PER_SECOND + rand() % (7 * TICKS_PER_SECOND);
    }

    active_mode_->Tick();
}

void GameManager::LoadMap(const std::string &name) {
    FrontEnd_SetState(FE_MODE_LOADING);

    if(active_map_ != nullptr) {

    }

    try {
        active_map_ = new Map(name);
    } catch(const std::runtime_error &e) {
        Error("Failed to load map, aborting!\n%s\n", e.what());
    }

    MapManifest* manifest = MapManager::GetInstance()->GetManifest(name);
    std::string sample_ext = "d";
    if(manifest->time != "day") {
        sample_ext = "n";
    }

    for(unsigned int i = 1, idx = 0; i < 4; ++i) {
        if(i < 3) {
            ambient_samples_[idx++] = AudioManager::GetInstance()->CacheSample(
                    "audio/amb_" + std::to_string(i) + sample_ext + ".wav", false);
        }
        ambient_samples_[idx++] = AudioManager::GetInstance()->CacheSample("audio/batt_s" + std::to_string(i) + ".wav", false);
        ambient_samples_[idx++] = AudioManager::GetInstance()->CacheSample("audio/batt_l" + std::to_string(i) + ".wav", false);
    }

    ambient_emit_delay_ = g_state.sim_ticks + rand() % 100;

    active_mode_ = new SPGameMode();
    // call StartRound; deals with spawning everything in and other mode specific logic
    active_mode_->StartRound();

    /* todo: we should actually pause here and wait for user input
     *       otherwise players won't have time to read the loading screen */
    FrontEnd_SetState(FE_MODE_GAME);
}

void GameManager::UnloadMap() {
    for(auto & ambient_sample : ambient_samples_) {
        delete ambient_sample;
    }

    delete active_mode_;
    delete active_map_;
}

Player* GameManager::GetCurrentPlayer() {
    if(active_mode_ == nullptr) {
        // todo: return local player... ?
        return nullptr;
    }

    return active_mode_->GetCurrentPlayer();
}

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

#include "GameMode.h"

class BaseGameMode : public IGameMode {
public:
    BaseGameMode();
    ~BaseGameMode() override;

    void Tick() override;

    void StartRound() override;
    void RestartRound() override;
    void EndRound() override;

    void PlayerJoined(Player* player) override;
    void PlayerLeft(Player* player) override;

    void SpectatorJoined(Player* player) override;
    void SpectatorLeft(Player* player) override;

    unsigned int GetMaxSpectators() const override;
    unsigned int GetMaxPlayers() const override;

protected:
    void StartTurn() override;
    void EndTurn() override;

    void SpawnActors() override;
    void DestroyActors() override;

 private:
};

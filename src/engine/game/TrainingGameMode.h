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

#include "BaseGameMode.h"

class TrainingGameMode : public BaseGameMode {
public:

    std::string GetDescription() override { return "training"; }

    void StartMode() override;
    void EndMode() override;

    uint8_t GetMaxSpectators() override { return 0; }
    uint8_t GetMaxPlayers() override { return 1; }

    void StartTurn() override;
    void EndTurn() override;

    void StartRound() override;
    void RestartRound() override;
    void EndRound() override;

    void ChangeMap(const std::string &name) override;

protected:
private:
};

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

#include "mode_interface.h"

class GameMode : public IGameMode {
public:
	explicit GameMode( const GameModeDescriptor &descriptor );
	~GameMode() override;

	void Tick() override;

	void StartRound() override;
	void RestartRound() override;
	void EndRound() override;

	void PlayerJoined( Player *player ) override;
	void PlayerLeft( Player *player ) override;

	void SpectatorJoined( Player *player ) override;
	void SpectatorLeft( Player *player ) override;

	unsigned int GetMaxSpectators() const override;
	unsigned int GetMaxPlayers() const override;

	Player *GetCurrentPlayer() override;
	Actor *GetPossessedActor();
	void CyclePlayers();

	void AssignActorToPlayer( Actor *target, Player *owner ) override;

protected:
	void StartTurn( Player *player ) override;
	void EndTurn( Player *player ) override;

	void SpawnActors() override;
	void DestroyActors() override;

private:
};

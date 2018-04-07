/* OpenHoW
 * Copyright (C) 2017-2018 Mark Sowden <markelswo@gmail.com>
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
#include <pork/pork.h>

#include "pork_engine.h"
#include "server/server_actor.h"

void ClearPlayers(void) {
    memset(g_state.players, 0, sizeof(Player) * MAX_PLAYERS);
}

void InitPlayers(void) {
    LogDebug("initializing player data\n");

    ClearPlayers();
}

//////////////////////////////////////////////////////////////

Actor *Player_GetPig(Player *self, unsigned int slot) {
    pork_assert(self != NULL);

    if(slot >= MAX_PIGS) {
        LogWarn("failed to grab pig, slot %d exceeds limit %d, ignoring!\n", slot, MAX_PIGS);
        return NULL;
    }

    Actor *pig = self->pigs[slot];
    if(pig == NULL) {
        LogWarn("failed to grab pig, invalid slot %d, ignoring!\n", slot);
        return NULL;
    }

    return pig;
}

void Player_AssignPig(Player *self, Actor *pig) {
    pork_assert(self != NULL);

    if(pig == NULL) {
        LogWarn("attempted to assign an invalid pig, ignoring!\n");
        return;
    }

    if(pig->team != self->team) {
        LogWarn("pig isn't on same team as player, ignoring!\n");
        return;
    }

    self->pigs[++self->num_pigs] = pig;
}

/* doesn't actually remove the pig from the game, just removes
 * it from the players list, so they can no longer possess it.
 */
void Player_RemovePig(Player *self, unsigned int slot) {
    Actor *pig = Player_GetPig(self, slot);
    if(pig == NULL) {
        return;
    }

    if(self->current_pig == slot) {
        Actor_Depossess(pig, self);
    }
}


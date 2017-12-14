/* OpenHOW
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

#include "player.h"

Player g_players[MAX_PLAYERS];

/* Suggest changing ine 107/108 to player->pigs[slot] = player->pigs[--(player->num_pigs)];
 * Then you can ditch the +1 in the qsort call and the comparator doesn't need to handle NULL pointers
 * Probably don't even need the qsort actually
 */

void InitPlayers(void) {
    print_debug("initializing player data\n");

    ClearPlayers();
}

void ClearPlayers(void) {
    memset(g_players, 0, sizeof(Player) * MAX_PLAYERS);
}
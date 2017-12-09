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
#include "map.h"

/////////////////////////////////////////////
// WATER

#define WATER_TILES   512
float water_grid[WATER_TILES][WATER_TILES];

void SimulateWater(void) {
    if(g_state.game.is_paused) {
        return;
    }
}

void DrawWater(void) {
    static PLMesh *water_mesh = NULL;
    if(!water_mesh) {
        if (!(water_mesh = plCreateMesh(PLMESH_TRIANGLE_STRIP, PL_DRAW_IMMEDIATE, WATER_TILES * 2,
                                        2 * WATER_TILES + 2))) {
            PRINT_ERROR("Failed to create water mesh!\n");
        }
    }

    plDrawMesh(water_mesh);
}

////////////////////////////////////////////

void InitializeMap(void) {
}

void LoadMap(const char *path) {
}

void DrawMap(void) {
    DrawWater();
}

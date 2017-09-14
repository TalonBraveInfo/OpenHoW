/*  Copyright (C) 2017 Mark E Sowden    */

#include "map.h"

#include "formats/pog.h"

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

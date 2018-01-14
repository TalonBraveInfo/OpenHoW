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
#pragma once

typedef struct CLActor {
    bool is_reserved;
    bool is_visible;

    uint32_t type;

    int16_t health;
    struct {
        uint32_t item_id;
        int16_t quantity;
    } inventory[MAX_INVENTORY];
    uint8_t current_item;

    PLVector3 position;
    PLVector3 angles;
    PLVector3 bounds;

    PLModel *model;
    uint8_t eyes_frame;
    uint8_t gob_frame;

    /* animation */
    PLAnimation *cur_animation;
} CLActor;

void DrawActors(double delta);
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

// Physics Declarations

enum {
    /* ...original... */
    PHYS_BOUNDS_BOX     = 0,
    PHYS_BOUNDS_PRISM   = 1,
    PHYS_BOUNDS_SPHERE  = 2,
    PHYS_BOUNDS_NONE    = 3,
    /* ...any new types below... */

    MAX_PHYS_BOUND_TYPES
};

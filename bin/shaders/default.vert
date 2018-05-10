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

#include "./shared.vert"

/* some random comments
 * and some more
 * and some more
 */

// single line comment

void main() {

    /* fudged up crap here to test how the parser deals with spaces :) */
    vec2             stuff =         vec2(2.0, 2.0);

    gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}
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
#pragma once

#include <PL/platform_math.h>

/* extension of Platform's math functionality
 * a lot of these are borrowed from ioq3 to fill gaps
 */

inline static void Mat4Copy(PLMatrix in, PLMatrix *out) {
    memcpy(out, &in, sizeof(PLMatrix));
}

inline static void Mat4Multiply(PLMatrix in1, PLMatrix in2, PLMatrix *out) {
    out->m[ 0] = in1.m[ 0] * in2.m[ 0] + in1.m[ 4] * in2.m[ 1] + in1.m[ 8] * in2.m[ 2] + in1.m[12] * in2.m[ 3];
    out->m[ 1] = in1.m[ 1] * in2.m[ 0] + in1.m[ 5] * in2.m[ 1] + in1.m[ 9] * in2.m[ 2] + in1.m[13] * in2.m[ 3];
    out->m[ 2] = in1.m[ 2] * in2.m[ 0] + in1.m[ 6] * in2.m[ 1] + in1.m[10] * in2.m[ 2] + in1.m[14] * in2.m[ 3];
    out->m[ 3] = in1.m[ 3] * in2.m[ 0] + in1.m[ 7] * in2.m[ 1] + in1.m[11] * in2.m[ 2] + in1.m[15] * in2.m[ 3];

    out->m[ 4] = in1.m[ 0] * in2.m[ 4] + in1.m[ 4] * in2.m[ 5] + in1.m[ 8] * in2.m[ 6] + in1.m[12] * in2.m[ 7];
    out->m[ 5] = in1.m[ 1] * in2.m[ 4] + in1.m[ 5] * in2.m[ 5] + in1.m[ 9] * in2.m[ 6] + in1.m[13] * in2.m[ 7];
    out->m[ 6] = in1.m[ 2] * in2.m[ 4] + in1.m[ 6] * in2.m[ 5] + in1.m[10] * in2.m[ 6] + in1.m[14] * in2.m[ 7];
    out->m[ 7] = in1.m[ 3] * in2.m[ 4] + in1.m[ 7] * in2.m[ 5] + in1.m[11] * in2.m[ 6] + in1.m[15] * in2.m[ 7];

    out->m[ 8] = in1.m[ 0] * in2.m[ 8] + in1.m[ 4] * in2.m[ 9] + in1.m[ 8] * in2.m[10] + in1.m[12] * in2.m[11];
    out->m[ 9] = in1.m[ 1] * in2.m[ 8] + in1.m[ 5] * in2.m[ 9] + in1.m[ 9] * in2.m[10] + in1.m[13] * in2.m[11];
    out->m[10] = in1.m[ 2] * in2.m[ 8] + in1.m[ 6] * in2.m[ 9] + in1.m[10] * in2.m[10] + in1.m[14] * in2.m[11];
    out->m[11] = in1.m[ 3] * in2.m[ 8] + in1.m[ 7] * in2.m[ 9] + in1.m[11] * in2.m[10] + in1.m[15] * in2.m[11];

    out->m[12] = in1.m[ 0] * in2.m[12] + in1.m[ 4] * in2.m[13] + in1.m[ 8] * in2.m[14] + in1.m[12] * in2.m[15];
    out->m[13] = in1.m[ 1] * in2.m[12] + in1.m[ 5] * in2.m[13] + in1.m[ 9] * in2.m[14] + in1.m[13] * in2.m[15];
    out->m[14] = in1.m[ 2] * in2.m[12] + in1.m[ 6] * in2.m[13] + in1.m[10] * in2.m[14] + in1.m[14] * in2.m[15];
    out->m[15] = in1.m[ 3] * in2.m[12] + in1.m[ 7] * in2.m[13] + in1.m[11] * in2.m[14] + in1.m[15] * in2.m[15];
}

inline static void Mat4Translation(PLVector3 vec, PLMatrix *out) {
    out->m[ 0] = 1.0f; out->m[ 4] = 0.0f; out->m[ 8] = 0.0f; out->m[12] = vec.x;
    out->m[ 1] = 0.0f; out->m[ 5] = 1.0f; out->m[ 9] = 0.0f; out->m[13] = vec.y;
    out->m[ 2] = 0.0f; out->m[ 6] = 0.0f; out->m[10] = 1.0f; out->m[14] = vec.z;
    out->m[ 3] = 0.0f; out->m[ 7] = 0.0f; out->m[11] = 0.0f; out->m[15] = 1.0f;
}

inline static void Mat4Ortho(float left, float right, float bottom, float top, float znear, float zfar, PLMatrix *out) {
    out->m[ 0] = 2.0f / (right - left); out->m[ 4] = 0.0f;                  out->m[ 8] = 0.0f;                  out->m[12] = -(right + left) / (right - left);
    out->m[ 1] = 0.0f;                  out->m[ 5] = 2.0f / (top - bottom); out->m[ 9] = 0.0f;                  out->m[13] = -(top + bottom) / (top - bottom);
    out->m[ 2] = 0.0f;                  out->m[ 6] = 0.0f;                  out->m[10] = 2.0f / (zfar - znear); out->m[14] = -(zfar + znear) / (zfar - znear);
    out->m[ 3] = 0.0f;                  out->m[ 7] = 0.0f;                  out->m[11] = 0.0f;                  out->m[15] = 1.0f;
}
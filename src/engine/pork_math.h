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

#include <PL/platform_math.h>

/* extension of Platform's math functionality
 * a lot of these are borrowed from ioq3 to fill gaps */

#ifdef __cplusplus

inline static void VecAngleClamp(PLVector3* vector) {
  for (unsigned int i = 0; i < 3; ++i) {
    if ((*vector)[i] >= 360) {
      (*vector)[i] = 0;
    } else if ((*vector)[i] <= -360) {
      (*vector)[i] = 0;
    }
  }
}

#endif

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include <plcore/pl_math.h>

/* extension of Platform's math functionality */

#ifdef __cplusplus

inline static void VectorClamp( hei::Vector3 *vector, float min, float max ) {
	for ( unsigned int i = 0; i < 3; ++i ) {
		if ( ( *vector )[ i ] >= max ) {
			( *vector )[ i ] = max;
		} else if ( ( *vector )[ i ] <= min ) {
			( *vector )[ i ] = min;
		}
	}
}

inline static void VecAngleClamp( hei::Vector3 *vector ) {
	for ( unsigned int i = 0; i < 3; ++i ) {
		if ( ( *vector )[ i ] >= 360 ) {
			( *vector )[ i ] = 0;
		} else if ( ( *vector )[ i ] <= -360 ) {
			( *vector )[ i ] = 0;
		}
	}
}

#endif

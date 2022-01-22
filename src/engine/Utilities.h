// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#include <string>
#include <algorithm>

#include "MathUtilities.h"

#define u_unused( ... ) (void)( __VA_ARGS__ )

#define u_fclose( FILE )  if((FILE) != NULL) { fclose((FILE)); (FILE) = NULL; }
#define u_free( DATA )    free((DATA)); (DATA) = NULL

static inline std::string u_stringtolower( std::string s ) {
	std::transform( s.begin(), s.end(), s.begin(), ::tolower );
	return s;
}

PL_EXTERN_C

void *u_alloc( size_t num, size_t size, bool abort_on_fail );

const char *u_scan( const char *path, const char **preference );
const char *u_find2( const char *path, const char **preference, bool abort_on_fail );

char *u_new_filename( char *dst, const char *src, const char *ext );

PL_EXTERN_C_END

#ifdef _DEBUG
#   define u_assert( a, ... ) if(!((a))) { Warning(__VA_ARGS__); Print("Assertion hit in \"%s\" on line %d\n", PL_FUNCTION, __LINE__); } assert((a))
#else
#   define u_assert(a, ...) if(!((a))) { LogWarn(__VA_ARGS__); }
#endif

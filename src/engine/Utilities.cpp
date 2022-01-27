// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "App.h"

/****************************************************/
/* Memory */

unsigned long u_unmangle( void *source, void *destination ) {
	/* todo */

	return 0;
}

/* todo: kill me */
void *u_alloc( size_t num, size_t size, bool abort_on_fail ) {
	return PlCAlloc( num, size, abort_on_fail );
}

/****************************************************/
/* Filesystem */

const char *u_scan( const char *path, const char **preference ) {
	static char find[PL_SYSTEM_MAX_PATH];
	while ( *preference != NULL ) {
		snprintf( find, sizeof( find ), "%s.%s", path, *preference );
		if ( PlFileExists( find ) ) {
			//LogDebug( "Found \"%s\"\n", find );
			return find;
		}
		preference++;
	}

	DebugMsg( "Failed to find \"%s\"\n", path );
	return "";
}

const char *u_find2( const char *path, const char **preference, bool abort_on_fail ) {
	static char out[PL_SYSTEM_MAX_PATH];
	memset( out, 0, sizeof( out ) );

	strncpy( out, u_scan( path, preference ), sizeof( out ) );
	if ( *out == '\0' ) {
		if ( abort_on_fail ) {
			Error( "Failed to find \"%s\"!\n", path );
		}

		Warning( "Failed to find \"%s\"!\n", path );
		return NULL;
	}

	//LogDebug( "Found \"%s\"\n", out );
	return out;
}

char *u_new_filename( char *dst, const char *src, const char *ext ) {
	strncpy( dst, src, strlen( src ) - 3 );
	dst[ strlen( src ) - 3 ] = '\0';
	strcat( dst, ext );
	return dst;
}

/* OpenHoW
 * Copyright (C) 2017-2020 TalonBrave.info and Others (see CONTRIBUTORS)
 * Copyright (C) 2017 Daniel Collins <solemnwarning@solemnwarning.net>
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

#include <plcore/pl_package.h>
#include <plcore/pl_image.h>
#include <plcore/pl_console.h>

extern unsigned int MSG_LVL_PRINT;
extern unsigned int MSG_LVL_WARN;
extern unsigned int MSG_LVL_ERR;

#define Print( ... )    PlLogMessage( MSG_LVL_PRINT, __VA_ARGS__ )
#define Warning( ... )  PlLogMessage( MSG_LVL_WARN, __VA_ARGS__ )
#define Error( ... )    PlLogMessage( MSG_LVL_ERR, __VA_ARGS__ ); exit( EXIT_FAILURE )

typedef enum ERegion {
	REGION_UNKNOWN = -1,

	REGION_ENG,
	REGION_FRE,
	REGION_GER,
	REGION_ITA,
	REGION_RUS,
	REGION_SPA,

	MAX_REGIONS
} ERegion;
extern const char *region_idents[MAX_REGIONS];

typedef enum EPlatform {
	PLATFORM_UNKNOWN = -1,
	PLATFORM_PSX,               /* PSX version */
	PLATFORM_PC,                /* PC version */
	PLATFORM_PC_DIGITAL,        /* PC/Digital version */
} EPlatform;

typedef struct VersionInfo {
	ERegion region;
	EPlatform platform;
} VersionInfo;
extern VersionInfo version_info;

void CheckGameVersion( const char *path );

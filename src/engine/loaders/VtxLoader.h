// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#pragma once

#define VTX_MAX_VERTICES  4096

PL_EXTERN_C

typedef struct VtxHandle {
	struct PLGVertex *vertices;
	unsigned int num_vertices;
} VtxHandle;

VtxHandle *Vtx_LoadFile( const char *path );
void Vtx_DestroyHandle( VtxHandle *handle );

PL_EXTERN_C_END

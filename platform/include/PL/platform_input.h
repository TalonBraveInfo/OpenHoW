
#pragma once

#include <PL/platform.h>

#define PLINPUT_MOUSE_LEFT      (1 << 0)
#define PLINPUT_MOUSE_RIGHT     (1 << 1)
#define PLINPUT_MOUSE_MIDDLE    (1 << 2)

PL_EXTERN_C

PL_EXTERN void plClearKeyboardState(void);

PL_EXTERN_C_END
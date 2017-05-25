/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include "PL/platform_math.h"

// GLM
#if 0
#   ifndef PLATFORM_EXCLUDE_GLM
#       ifdef _MSC_VER
#           pragma warning(disable: 4201)
#       endif

#       include <glm/gtc/type_ptr.hpp>
#       include <glm/vec2.hpp>
#       include <glm/vec3.hpp>
#       include <glm/vec4.hpp>
#       include <glm/mat4x4.hpp>
#       include <glm/gtc/matrix_transform.hpp>

#       ifdef _MSC_VER
#           pragma warning(default: 4201)
#       endif
#   endif
#endif

#ifdef _DEBUG

void _plDebugVector3D(PLVector3D *v, PLVector3D *v3) {
    PLVector3D v2(0, 0, 1), v4;

    if(*v == v2) {

    } else if(v == v3) {

    } else if(v3 == v) {

    }

    printf("%s %s %s\n", plPrintVector3D(*v), plPrintVector3D(v2), plPrintVector3D(*v3));

    v2[0] = 2;
    v2[1] = 3;
    v2[2] = 0;

    printf("%s %s %s\n", plPrintVector3D(*v), plPrintVector3D(v2), plPrintVector3D(*v3));

    *v = v2;
    v2 *= v4;
    v2 *= *v;
    *v *= v2;

    printf("%s %s %s\n", plPrintVector3D(*v), plPrintVector3D(v2), plPrintVector3D(*v3));

    if(v2 != v4) {

    } else if(v != v3) {

    } else if(v3 != v) {

    } else if(v2 != *v) {

    } else if(*v != v2) {

    }

    printf("%s %s %s\n", plPrintVector3D(*v), plPrintVector3D(v2), plPrintVector3D(*v3));

    plAddVector3D(v, v2);

    printf("%s (including %s)\n", plPrintVector3D(*v), plPrintVector3D(v2));
}

void _plDebugMath(void) {
plFunctionStart();

    PLVector3D v1, v2;
    _plDebugVector3D(&v1, &v2);

    // Debug colours...

    // Set up standard RGB colours.
    PLColour red = { PL_COLOUR_RED };
    PLColour green(PL_COLOUR_GREEN);
    PLColour blue = { PL_COLOUR_BLUE };

    // Customised vars mixing our standard RGB colours.
    PLColour c0 = red / green;
    PLColour c1 = green * red;

    // Alterations to our first customised var, c0.
    c0 += green;
    c0 *= green + blue;
    c0 /= green;

    // Print out all our variables.
    printf("%s", plPrintColour(c0));
    printf("%s", plPrintColour(c1));
    printf("%s", plPrintColour(red));
    printf("%s", plPrintColour(green));
    printf("%s", plPrintColour(blue));

    plFunctionEnd();
}

#endif

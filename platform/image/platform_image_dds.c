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

#include "PL/platform_image.h"

struct DDSPixelFormat {
    uint32_t size;
    uint32_t flags;
    uint32_t fourcc;
    uint32_t rgbbitcount;
};

typedef struct DDSHeader {
    uint32_t size;          // Should always be 124.
    uint32_t flags;
    uint32_t height, width;
    uint32_t pitchlinear;
    uint32_t depth;
    uint32_t levels;
    uint32_t reserved1[11];

    //

    uint32_t caps, caps2, caps3, caps4;
    uint32_t reserved2;
} DDSHeader;

enum DDSFlag {
    DDS_CAPS,
};

bool _plDDSFormatCheck(FILE *fin) {
    rewind(fin);

    PLchar ident[4];
    fread(ident, sizeof(PLchar), 4, fin);

    return (bool)(strncmp(ident, "DDS", 3) == 0);
}

PLresult _plLoadDDSImage(FILE *fin, PLImage *out) {
    DDSHeader header;
    memset(&header, 0, sizeof(DDSHeader));
    if(fread(&header, sizeof(DDSHeader), 1, fin) != 1) {
        return PL_RESULT_FILEREAD;
    }

    memset(out, 0, sizeof(PLImage));

    out->width = header.width;
    out->height = header.height;

    return PL_RESULT_SUCCESS;
}

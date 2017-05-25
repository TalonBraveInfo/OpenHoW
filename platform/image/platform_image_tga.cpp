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

typedef struct TGAHeader {
    PLbyte      id_length;
    PLbyte      cm_type;
    PLbyte      image_type;
    PLushort    cm_start;
    PLushort    cm_length;
    PLbyte      cm_depth;
    PLushort    x_offset;
    PLushort    y_offset;
    PLushort    width;
    PLushort    height;
    PLbyte      pixel_depth;
    PLbyte      image_desc;
} TGAHeader;

enum TGAType {
    TGA_TYPE_NONE,

    TGA_TYPE_COLOURMAPPED,
    TGA_TYPE_TRUECOLOUR,
    TGA_TYPE_MONOCHROME,

    TGA_TYPE_COLOURMAPPED_ENCODED = 9,
    TGA_TYPE_TRUECOLOUR_ENCODED = 10,
    TGA_TYPE_MONOCHROME_ENCODED
} TGAType;

PLresult _plLoadTGAImage(FILE *fin, PLImage *out) {
    TGAHeader header;
    memset(&header, 0, sizeof(TGAHeader));
    if (fread(&header, sizeof(TGAHeader), 1, fin) != 1) {
        return PL_RESULT_FILEREAD;
    } else if (!plIsValidImageSize(header.width, header.height)) {
        return PL_RESULT_IMAGERESOLUTION;
    }

    memset(out, 0, sizeof(PLImage));

    out->width = header.width;
    out->height = header.height;
    if(header.pixel_depth == 24) {
        out->colour_format = PL_COLOURFORMAT_RGBA;
        out->format = PL_IMAGEFORMAT_RGBA4;
    } else {
        out->colour_format = PL_COLOURFORMAT_RGB;
        out->format = PL_IMAGEFORMAT_RGB4;
    }
    out->size = header.pixel_depth * header.width * header.height;
    out->levels = 1;

    out->data = new PLbyte*[out->levels];
    out->data[0] = new PLbyte[out->size];

    switch(header.image_type) {
        case TGA_TYPE_MONOCHROME:
        case TGA_TYPE_COLOURMAPPED:
        case TGA_TYPE_TRUECOLOUR: {
            if(fread(out->data[0], out->size, 1, fin) != out->size) {
                _plFreeImage(out);
                return PL_RESULT_FILEREAD;
            }

            for(PLuint swap = 0; swap < (PLint)out->size; swap += 4) {

            }
            break;
        }

        case TGA_TYPE_MONOCHROME_ENCODED:
        case TGA_TYPE_COLOURMAPPED_ENCODED:
        case TGA_TYPE_TRUECOLOUR_ENCODED: {
            break;
        }

        default:break;
    }

    return PL_RESULT_SUCCESS;
}


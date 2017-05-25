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

#include <PL/platform_image.h>

/* Bitmap image format
 * I'm only going to support the widely used BM type here.
 */

typedef struct __attribute((packed)) BMPHeader {
    uint32_t size;
    uint32_t reserved;
    uint32_t offset;
} BMPHeader;

typedef struct __attribute((packed)) BMPInfoHeader {
    uint32_t size;          // Number of bytes required for this struct.
    int32_t width;          // Width (in pixels)
    int32_t height;         // Height (in pixels)
    uint16_t planes;        // Should always return as 1.
    uint16_t bit_count;     // Number of bits per pixel (0, 1, 4, 8, 16, 24, 32).
    uint32_t compression;
    uint32_t size_image;    // Size (in bytes) of the image, this might be 0 for RGB types.

    int32_t xppm;
    int32_t yppm;

    uint32_t palette_colours;
    uint32_t palette_important;
} BMPInfoHeader;

bool _plBMPFormatCheck(FILE *fin) {
    rewind(fin);

    uint8_t ident[2];
    if(fread(&ident, sizeof(uint8_t), 2, fin) != 2) {
        return false;
    }

    return (bool)(strncmp((const char*)ident, "BM", 2) == 0);
}

PLresult _plLoadBMPImage(FILE *fin, PLImage *out) {
    BMPHeader header;
    if (fread(&header, sizeof(BMPHeader), 1, fin) != 1) {
        return PL_RESULT_FILEREAD;
    }

    if(!header.size) {
        return PL_RESULT_FILESIZE;
    }

    memset(out, 0, sizeof(PLImage));



    return PL_RESULT_SUCCESS;
}
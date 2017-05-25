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

/*	Monolith's DTX Format (http://www.cnblogs.com/crsky/p/4702916.html)	*/

typedef struct {
    PLint32 version;    // Version of the format, Lithtech used negative numbers.

    PLuint16 width, height;     // Width and height of the texture.
    PLuint16 mipmaps;           // Number of mipmaps included.
    PLuint16 sections;

    PLint32 flags, userflags;
    PLbyte extra[12];
    PLchar commandstring[128];
} DTXHeader;

typedef struct {
    // no idea.
} DTXSection;

#define DTX_VERSION_2   -2 // Lithtech 1.0 (Shogo)
// Lithtech 1.5
// Lithtech 2.0
// Lithtech 2.2
// Lithtech 2.4
#define DTX_VERSION_5   -5 // Lithtech Talon (Purge)
// Lithtech Jupiter
// Lithtech Jupiter EX
#define DTX_VERSION_MIN     DTX_VERSION_2
#define DTX_VERSION_MAX     DTX_VERSION_5

#define DTX_GROUP(a)    a.extra[0]
#define DTX_MIPMAP(a)   a.extra[1]
#define DTX_FORMAT(a)   a.extra[2]
#define DTX_OFFSET(a)   a.extra[3]

enum DTXFlag {
    DTX_FLAG_FULLBRIGHT = (1 << 0),
    DTX_FLAG_16BIT = (1 << 1),
    DTX_FLAG_4444 = (1 << 7),
    DTX_FLAG_5551 = (1 << 8),
    DTX_FLAG_CUBEMAP = (1 << 10),
    DTX_FLAG_NORMALMAP = (1 << 11),
} DTXFlag;

enum DTXFormat {
    DTX_FORMAT_8PALLETTE,
    DTX_FORMAT_8,
    DTX_FORMAT_16,
    DTX_FORMAT_32,

    // Compressed Formats
    DTX_FORMAT_S3TC_DXT1,
    DTX_FORMAT_S3TC_DXT3,
    DTX_FORMAT_S3TC_DXT5,
} DTXFormat;

PLbyte _plGetDTXFormat(DTXHeader *dtx) {
    // This is a little hacky, DTX version 2 images don't seem to use
    // the extra[2] slot the same way as later versions. So we need to
    // basically switch it to 0 since 99.9% of textures from that period
    // use a pallette anyway.
    //
    // tl;dr this is a hack because I'm lazy!
    if (dtx->version >= DTX_VERSION_2)
        return DTX_FORMAT_8PALLETTE;

    return dtx->extra[2];
}

bool _plDTXFormatCheck(FILE *fin) {
    rewind(fin);

    // Try reading in the type first, as Lithtech has "resource types" rather than idents.
    int type;
    if((fread(&type, sizeof(int), 1, fin) != 1)) {
        return false;
    }

    return (type == 0);
}

PLresult _plLoadDTXImage(FILE *fin, PLImage *out) {
    plSetErrorFunction("_plLoadDTXImage");

    DTXHeader header;
    memset(&header, 0, sizeof(header));
    if (fread(&header, sizeof(DTXHeader), 1, fin) != 1)
        return PL_RESULT_FILEREAD;
    else if ((header.version < DTX_VERSION_MAX) || (header.version > DTX_VERSION_MIN))
        return PL_RESULT_FILEVERSION;
    else if ((header.width < 8) || (header.height < 8))
        return PL_RESULT_IMAGERESOLUTION;

    memset(out, 0, sizeof(PLImage));

    out->width = header.width;
    out->height = header.height;

    switch (_plGetDTXFormat(&header)) {
        case DTX_FORMAT_8PALLETTE:
            out->size = header.width * header.height;
            out->format = PL_IMAGEFORMAT_RGB8;
            out->colour_format = PL_COLOURFORMAT_RGB;
            break;

        case DTX_FORMAT_S3TC_DXT1:
            out->size = (header.width * header.height) >> 1;
            out->format = PL_IMAGEFORMAT_RGB_DXT1;
            out->colour_format = PL_COLOURFORMAT_RGB;
            break;
        case DTX_FORMAT_S3TC_DXT3:
            out->size = header.width * header.height;
            out->format = PL_IMAGEFORMAT_RGBA_DXT3;
            out->colour_format = PL_COLOURFORMAT_RGBA;
            break;
        case DTX_FORMAT_S3TC_DXT5:
            out->size = header.width * header.height;
            out->format = PL_IMAGEFORMAT_RGBA_DXT5;
            out->colour_format = PL_COLOURFORMAT_RGBA;
            break;

        case DTX_FORMAT_8:
            out->size = header.width * header.height;
            out->format = PL_IMAGEFORMAT_RGB8;
            out->colour_format = PL_COLOURFORMAT_RGB;
            break;
        default:
        case DTX_FORMAT_16:
        case DTX_FORMAT_32:
            out->size = (PLuint)(header.width * header.height * 4);
            out->format = PL_IMAGEFORMAT_RGBA8;
            out->colour_format = PL_COLOURFORMAT_RGBA;
            break;
    }

    if (!out->size)
        return PL_RESULT_FILESIZE;

    for (int i = 0; i < header.mipmaps; i++) {
    }

    out->data = new PLbyte*[header.mipmaps];
    out->data[0] = new PLbyte[out->size];

    fread(out->data[0], sizeof(PLbyte), out->size, fin);

    /*	for (PLuint i = 0; i < (PLuint)size; i += 4)
    {
    image[i + 0] ^= image[i + 2];
    image[i + 2] ^= image[i + 0];
    image[i + 0] ^= image[i + 2];
    }*/

    return PL_RESULT_SUCCESS;
}
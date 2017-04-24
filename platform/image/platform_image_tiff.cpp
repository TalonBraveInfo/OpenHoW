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

#include "platform_image.h"

#include <tiffio.h>

bool _plTIFFFormatCheck(FILE *fin) {
    rewind(fin);

    PLchar ident[2];
    fread(ident, sizeof(PLchar), 2, fin);

    return (bool)((strncmp(ident, "II", 2) == 0) || (strncmp(ident, "MM", 2) == 0));
}

PLresult plWriteTIFFImage(const PLImage *image, const PLchar *path) {
    TIFF *tif = TIFFOpen(path, "w");
    if(!tif) {
        return PL_RESULT_FILEPATH;
    }

    PLuint spp = plGetSamplesPerPixel(image->colour_format);
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, image->width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, image->height);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    size_t line_bytes = spp * image->width;
    PLbyte *buf = (PLbyte*)_TIFFmalloc(TIFFScanlineSize(tif));
    if(!buf) {
        std::cout << "Failed to allocate memory for image buffer\n";

        TIFFClose(tif);

        return PL_RESULT_MEMORYALLOC;
    }

    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, image->width * spp));

    for(uint32 row = 0; row < image->height; row++) {
        memcpy(buf, &image->data[0][(image->height - row - 1) * line_bytes], line_bytes);
        if(TIFFWriteScanline(tif, buf, row, 0) < 0) {
            break;
        }
    }

    TIFFClose(tif);
    _TIFFfree(buf);

    return PL_RESULT_SUCCESS;
}

PLresult _plLoadTIFFImage(const PLchar *path, PLImage *out) {
    plFunctionStart();

    if(!plIsValidString(path)) {
        return PL_RESULT_FILEPATH;
    }

    TIFF *tif = TIFFOpen(path, "r");
    if(!tif) {
        return PL_RESULT_FILEREAD;
    }

    TIFFRGBAImage image;
    memset(&image, 0, sizeof(TIFFRGBAImage));

    PLchar error_message[1024];
    if(!TIFFRGBAImageBegin(&image, tif, 0, error_message)) {
        plSetError("TIFFRGBAImageBegin failed");

        TIFFClose(tif);
        return PL_RESULT_FILEREAD;
    }

    size_t pixels = image.width * image.height;
    uint32 *raster = (uint32*)_TIFFmalloc(pixels * sizeof(uint32));
    if(!raster) {
        TIFFClose(tif);
        return PL_RESULT_MEMORYALLOC;
    }

    if(!TIFFRGBAImageGet(&image, raster, image.width, image.height)) {
        plSetError("TIFFReadRGBAImage failed");

        TIFFClose(tif);
        _TIFFfree(raster);
        return PL_RESULT_FILEREAD;
    }

    memset(out, 0, sizeof(PLImage));
    out->size           = image.width * image.height * 4;
    out->levels         = 1;
    out->colour_format  = PL_COLOURFORMAT_RGBA;
    out->data           = new PLbyte*[1];
    out->data[0]        = new PLbyte[out->size];
    out->format         = PL_IMAGEFORMAT_RGBA8;
    out->width          = image.width;
    out->height         = image.height;

    // todo, data needs to contain raster... somehow
    memcpy(out->data[0], raster, out->size);

    TIFFRGBAImageEnd(&image);
    TIFFClose(tif);

    _TIFFfree(raster);

    return PL_RESULT_SUCCESS;
}

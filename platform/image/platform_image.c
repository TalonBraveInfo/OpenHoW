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
#include "PL/platform_filesystem.h"
#include <PL/platform_math.h>

PLresult plLoadImagef(FILE *fin, const char *path, PLImage *out) {
    if(!fin) {
        plSetError("invalid file handle");
        return PL_RESULT_FILEREAD;
    }

    PLresult result = PL_RESULT_FILETYPE;
    if(_plDDSFormatCheck(fin)) {
        result = _plLoadDDSImage(fin, out);
    } else if(_plTIMFormatCheck(fin)) {
        result = _plLoadTIMImage(fin, out);
    } else if(_plVTFFormatCheck(fin)) {
        result = _plLoadVTFImage(fin, out);
    } else if(_plDTXFormatCheck(fin)) {
        result = _plLoadDTXImage(fin, out);
    } else if(_plTIFFFormatCheck(fin)) {
        result = _plLoadTIFFImage(path, out);
    } else if(_plBMPFormatCheck(fin)) {
        result = _plLoadBMPImage(fin, out);
    } else {
        const PLchar *extension = plGetFileExtension(path);
        if(plIsValidString(extension)) {
            if (!strncmp(extension, PLIMAGE_EXTENSION_FTX, 3)) {
                result = _plLoadFTXImage(fin, out);
            } else if (!strncmp(extension, PLIMAGE_EXTENSION_PPM, 3)) {
                result = _plLoadPPMImage(fin, out);
            }
        }
    }

    if(result == PL_RESULT_SUCCESS) {
        strncpy(out->path, path, sizeof(out->path));
    }

    return result;
}

PLresult plLoadImage(const PLchar *path, PLImage *out) {
    plFunctionStart();

    if (!plIsValidString(path)) {
        return PL_RESULT_FILEPATH;
    }

    FILE *fin = fopen(path, "rb");
    if(!fin) {
        return PL_RESULT_FILEREAD;
    }

    if(strrchr(path, ':')) {
        // Very likely a packaged image.
        // example/package.wad:myimage
    }

    PLresult result = plLoadImagef(fin, path, out);

    fclose(fin);

    return result;
}

PLresult plWriteImage(const PLImage *image, const PLchar *path) {
    plFunctionStart();

    if (!plIsValidString(path)) {
        return PL_RESULT_FILEPATH;
    }

    PLresult result = PL_RESULT_FILETYPE;

    const PLchar *extension = plGetFileExtension(path);
    if(plIsValidString(extension)) {
        if (!strncmp(extension, PLIMAGE_EXTENSION_TIFF, 3)) {
            result = plWriteTIFFImage(image, path);
        } else {
            // todo, Write BMP or some other easy-to-go format.
        }
    }

    return result;
}

// Returns the number of samples per-pixel depending on the colour format.
PLuint plGetSamplesPerPixel(PLColourFormat format) {
    plFunctionStart();

    switch(format) {
        case PL_COLOURFORMAT_ABGR:
        case PL_COLOURFORMAT_ARGB:
        case PL_COLOURFORMAT_BGRA:
        case PL_COLOURFORMAT_RGBA:
            return 4;
        case PL_COLOURFORMAT_BGR:
        case PL_COLOURFORMAT_RGB:
            return 3;
    }

    return 0;
}

void plConvertImageFormat(PLImage *image, PLColourFormat dest_colour, PLImageFormat dest_pixel) {
    switch(image->format) {
        default: {
            return;
        }

        case PL_IMAGEFORMAT_RGB5A1: {

        }
    }
}

PLuint _plGetImageSize(PLImageFormat format, PLuint width, PLuint height) {
    plFunctionStart();

    switch(format) {
        case PL_IMAGEFORMAT_RGB_DXT1:   return (width * height) >> 1;
        case PL_IMAGEFORMAT_RGBA_DXT1:  return width * height * 4;
        case PL_IMAGEFORMAT_RGBA_DXT3:
        case PL_IMAGEFORMAT_RGBA_DXT5:  return width * height;

        case PL_IMAGEFORMAT_RGB5A1:     return width * height * 2;
        case PL_IMAGEFORMAT_RGB8:
        case PL_IMAGEFORMAT_RGB565:     return width * height * 3;
        case PL_IMAGEFORMAT_RGBA4:
        case PL_IMAGEFORMAT_RGBA8:      return width * height * 4;
        case PL_IMAGEFORMAT_RGBA16F:
        case PL_IMAGEFORMAT_RGBA16:     return width * height * 8;

        default:    return 0;
    }
}

void _plAllocateImage(PLImage *image, PLuint size, PLuint levels) {
    image->data = (PLbyte**)calloc(levels, sizeof(PLbyte));
}

void _plFreeImage(PLImage *image) {
    plFunctionStart();

    if (!image || !image->data) {
        return;
    }

    for(PLuint levels = 0; levels < image->levels; ++levels) {
        if(!image->data[levels]) {
            continue;
        }

        free(image->data[levels]);
    }

    free(image->data);
}

PLbool plIsValidImageSize(PLuint width, PLuint height) {
    plFunctionStart();

    if((width < 2) || (height < 2)) {
        return false;
    } else if(!plIsPowerOfTwo(width) || !plIsPowerOfTwo(height)) {
        return false;
    }

    return true;
}

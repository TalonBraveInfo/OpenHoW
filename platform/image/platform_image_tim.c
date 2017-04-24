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

#include <platform_math.h>
#include "platform_image.h"

/*  http://rewiki.regengedanken.de/wiki/.TIM
 *  https://mrclick.zophar.net/TilEd/download/timgfx.txt
 */

typedef struct TIMHeader {
    uint32_t type;
    uint32_t offset;
} TIMHeader;

typedef struct TIMPaletteInfo {
    uint16_t palette_org_x;
    uint16_t palette_org_y;
    uint16_t palette_colours;
    uint16_t num_palettes;
} TIMPaletteInfo;

typedef struct TIMImageInfo {
    uint16_t org_x;
    uint16_t org_y;
    uint16_t width;
    uint16_t height;
} TIMImageInfo;

enum TIMType {
    TIM_TYPE_4BPP   = 0x08,
    TIM_TYPE_8BPP   = 0x09,
    TIM_TYPE_16BPP  = 0x02,
    TIM_TYPE_24BPP  = 0x03,
};

#define TIM_IDENT   16

bool _plTIMFormatCheck(FILE *fin) {
    rewind(fin);

    uint32_t ident;
    if(fread(&ident, sizeof(uint32_t), 1, fin) != 1) {
        return false;
    }

    return (bool)(ident == TIM_IDENT);
}

PLresult plWriteTIMImage(const PLImage *image, const char *path) {
    if(!plIsValidString(path)) {
        return PL_RESULT_FILEPATH;
    }

    return PL_RESULT_SUCCESS;
}

PLresult _plLoadTIMImage(FILE *fin, PLImage *out) {
    plFunctionStart();

    TIMHeader header;
    if (fread(&header, sizeof(TIMHeader), 1, fin) != 1) {
        return PL_RESULT_FILEREAD;
    }

    memset(out, 0, sizeof(PLImage));

    switch(header.type) {

        case TIM_TYPE_4BPP:
        case TIM_TYPE_8BPP: {
            TIMPaletteInfo palette_info;
            if(fread(&palette_info, sizeof(TIMPaletteInfo), 1, fin) != 1) {
                return PL_RESULT_FILEREAD;
            }

            uint16_t palettes[palette_info.num_palettes][palette_info.palette_colours];
            for(PLuint i = 0; i < palette_info.num_palettes; i++) {
                if(fread(palettes[i], sizeof(uint16_t), palette_info.palette_colours, fin) != palette_info.palette_colours) {
                    return PL_RESULT_FILEREAD;
                }
            }

            fseek(fin, 4, SEEK_CUR);

            TIMImageInfo image_info;
            if(fread(&image_info, sizeof(TIMImageInfo), 1, fin) != 1) {
                return PL_RESULT_FILEREAD;
            }

            // The width of the image depends on its type.
            out->width = (header.type == TIM_TYPE_4BPP ?
                                   (unsigned int) (image_info.width * 4) :
                                   (unsigned int) (image_info.width * 2));
            out->height = image_info.height;
            if(!out->width || !out->height) {
                return PL_RESULT_IMAGERESOLUTION;
            }

            uint32_t size;
            if(header.type == TIM_TYPE_4BPP) {
                size = (uint32_t)(out->width * out->height / 2);
            } else { // 8bpp
                size = out->width * out->height;
            }

            uint8_t img[size];
            if(fread(img, sizeof(uint8_t), size, fin) != size) {
                return PL_RESULT_FILEREAD;
            }

            out->format = PL_IMAGEFORMAT_RGB5A1;
            out->size = _plGetImageSize(out->format, out->width, out->height);
            out->levels = 1;

            out->data = (uint8_t**)calloc(out->levels, sizeof(uint8_t*));
            out->data[0] = (uint8_t*)calloc(out->size, sizeof(uint8_t));
#if 1 // copy out the palette...
            for(unsigned int i = 0; i < palette_info.palette_colours; i++) {
                ((uint16_t**)(out->data))[0][i] = palettes[0][i];
            }
#else // each 'pixel' within our image is actually an index...
            if(header.type == TIM_TYPE_4BPP) {
                for (unsigned int i = 0, k = 0; i < size; i++, k++) {
                    ((uint16_t**)(out->data))[0][k] = palettes[0][(img[i] & 0x0F)]; k++; //out->data[0][k] = (uint8_t)palettes[0][(img[i] & 0x0F)]; k++;
                    ((uint16_t**)(out->data))[0][k] = palettes[0][(img[i] & 0xF0)];      //out->data[0][k] = (uint8_t)palettes[0][(img[i] & 0xF0) >> 4];
                }
            } else { // 8bpp

            }
#endif

            break;
        }

        case TIM_TYPE_16BPP:
        case TIM_TYPE_24BPP: {
            fseek(fin, 4, SEEK_CUR);

            TIMImageInfo image_info;
            if(fread(&image_info, sizeof(TIMImageInfo), 1, fin) != 1) {
                return PL_RESULT_FILEREAD;
            }

            out->levels = 1;

            out->width = image_info.width;
            out->height = image_info.height;
            if(!out->width || !out->height) {
                return PL_RESULT_IMAGERESOLUTION;
            }

            break;
        }

        default: {
            return PL_RESULT_FILETYPE;
        }
    }

    out->colour_format = PL_COLOURFORMAT_ABGR;

    return PL_RESULT_SUCCESS;
}

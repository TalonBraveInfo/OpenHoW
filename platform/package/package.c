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

#include "package_private.h"

PLPackage *plLoadPackage(const char *path, bool precache) {
    if(!plFileExists(path)) {
        _plReportError(PL_RESULT_FILEREAD, "Failed to load package, %s!", path);
        return NULL;
    }

    PLPackage *package;

    const char *extension = plGetFileExtension(path);
    if(extension[0] != '\0') {
        if((!strcmp(extension, "mad") || !strcmp(extension, "mtd")) && (package = _plLoadMADPackage(path, precache)) != NULL) {
            return package;
        }
    } else { // probably less safe loading solution, your funeral!
        if((package = _plLoadMADPackage(path, precache)) != NULL) {
            return package;
        }
    }

    return NULL;
}

void plUnloadPackage(PLPackage *package) {
}

bool plLoadPackageFile(PLPackage *package, const char *file, const uint8_t **data, size_t *size) {
    for(unsigned int i = 0; i < package->table_size; ++i) {
        if(strcmp(file, package->table[i].name) == 0) {
            if(package->table[i].data == NULL) {
                FILE *fh = fopen(package->path, "rb");
                if (fh == NULL) {
                    return false;
                }

                if (!_plLoadMADPackageFile(fh, &(package->table[i]))) {
                    fclose(fh);
                    return false;
                }
            }

            *data = package->table[i].data;
            *size = package->table[i].length;

            return true;
        }
    }

    return false;
}

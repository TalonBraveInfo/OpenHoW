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

#if 0
PL_INLINE static void plGetStringExtension(char *out, const char *in, unsigned int length) {
    const char *s = strrchr(in, '.');type
    if(!s || s[0] == '\0') {
        return;
    }

    strncpy(out, s + 1, length);
}

PL_INLINE static void plStripStringExtension(char *out, const char *in, unsigned int length) {
    const char *s = strrchr(in, '.');
    while(in < s) {
        *out++ = *in++;
    }
    *out = 0;
}
#endif

char *pl_strtolower(char *s);
char *pl_strntolower(char *s, size_t n);
char *pl_strtoupper(char *s);
char *pl_strntoupper(char *s, size_t n);

char *pl_strcasestr(const char *s, const char *find);

int pl_strcasecmp(const char *s1, const char *s2);
int pl_strncasecmp(const char *s1, const char *s2, size_t n);

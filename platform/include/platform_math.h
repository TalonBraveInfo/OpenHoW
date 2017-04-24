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

#pragma once

#include "platform.h"

// Base Defines

#define PL_PI   3.14159265358979323846

enum {
    PL_X,
    PL_Y,
    PL_Z
};

enum {
    PL_WIDTH,
    PL_HEIGHT
};

enum {
    PL_PITCH,
    PL_YAW,
    PL_ROLL
};

enum {
    PL_RED,
    PL_GREEN,
    PL_BLUE,
    PL_ALPHA
};

#define plFloatToByte(a)    (PLbyte)round(a * 255)
#define plByteToFloat(a)    (a / (PLfloat)255)

// Vectors

typedef struct PLVector2D {
    PLfloat x, y;

#ifdef __cplusplus
    PLVector2D(PLfloat a, PLfloat b) : x(a), y(b) {}
    PLVector2D() : x(0), y(0) {}

    void operator=(PLVector2D a) {
        x = a.x;
        y = a.y;
    }

    void operator=(PLfloat a) {
        x = a;
        y = a;
    }

    void operator*=(PLVector2D a) {
        x *= a.x;
        y *= a.y;
    }

    void operator*=(PLfloat a) {
        x *= a;
        y *= a;
    }

    void operator/=(PLVector2D a) {
        x /= a.x;
        y /= a.y;
    }

    void operator/=(PLfloat a) {
        x /= a;
        y /= a;
    }

    void operator+=(PLVector2D a) {
        x += a.x;
        y += a.y;
    }

    PLbool operator==(PLVector2D a) const { return ((x == a.x) && (y == a.y)); }

    PLVector2D operator*(PLVector2D a) const { return PLVector2D(x * a.x, y * a.y); }

    PLVector2D operator*(PLfloat a) const { return PLVector2D(x * a, y * a); }

    PLVector2D operator/(PLVector2D a) const { return PLVector2D(x / a.x, y / a.y); }

    PLVector2D operator/(PLfloat a) const { return PLVector2D(x / a, y / a); }

    PLfloat Length() { return std::sqrt(x * x + y * y); }

    PLVector2D Normalize() {
        PLVector2D out;
        PLfloat length = Length();
        if (length != 0)
            out.Set(x / length, y / length);
        return out;
    }

    void PL_INLINE Set(PLfloat a, PLfloat b) {
        x = a;
        y = b;
    }

    void PL_INLINE Clear() {
        x = 0;
        y = 0;
    }
#endif
} PLVector2D;

PL_INLINE static PLVector2D plCreateVector2D(PLfloat s, PLfloat t) {
    PLVector2D v = { s, t };
    return v;
}

PL_INLINE static void plClearVector2D(PLVector2D *v) {
    memset(v, 0, sizeof(PLVector2D));
}

PL_INLINE static void plAddVector2D(PLVector2D *v, PLVector2D v2) {
    v->x = v2.x; v->y = v2.y;
}

PL_INLINE static void plDivideVector2D(PLVector2D *v, PLVector2D v2) {
    v->x /= v2.x; v->y /= v2.y;
}

PL_INLINE static PLbool plCompareVector2D(PLVector2D v, PLVector2D v2) {
    return ((v.x == v2.x) && (v.y == v2.y));
}

typedef struct PLVector3D {
    PLfloat x, y, z;

#ifdef __cplusplus
    PLVector3D(PLfloat _x, PLfloat _y, PLfloat _z) : x(_x), y(_y), z(_z) {}
    PLVector3D(PLfloat *v) {
        x = v[0]; y = v[1]; z = v[2];
    }
    PLVector3D() : x(0), y(0) {}

#if 0
    void PL_INLINE operator = (PLVector3D a) {
        x = a.x;
        y = a.y;
        z = a.z;
    }
#endif

    void PL_INLINE operator = (PLfloat a) {
        x = a;
        y = a;
        z = a;
    }

    void PL_INLINE operator *= (const PLVector3D &v) {
        x *= v.x; y *= v.y; z *= v.z;
    }

    void PL_INLINE operator *= (PLfloat a) {
        x *= a; y *= a; z *= a;
    }

    void PL_INLINE operator += (PLVector3D a) {
        x += a.x; y += a.y; z += a.z;
    }

    void PL_INLINE operator += (PLfloat a) {
        x += a; y += a; z += a;
    }

    PL_INLINE PLbool operator == (const PLVector3D &a) const {
        return ((x == a.x) && (y == a.y) && (z == a.z));
    }

    PL_INLINE PLbool operator == (PLfloat f) const {
        return ((x == f) && (y == f) && (z == f));
    }

    PLbool PL_INLINE operator != (const PLVector3D &a) const {
        return !(a == *this);
    }

    PLbool PL_INLINE operator != (PLfloat f) const {
        return ((x != f) && (y != f) && (z != f));
    }

    PLVector3D PL_INLINE operator * (PLVector3D a) const {
        return PLVector3D(x * a.x, y * a.y, z * a.z);
    }

    PLVector3D PL_INLINE operator * (PLfloat a) const {
        return PLVector3D(x * a, y * a, z * a);
    }

    PLVector3D PL_INLINE operator - (PLVector3D a) const {
        return PLVector3D(x - a.x, y - a.y, z - a.z);
    }

    PLVector3D PL_INLINE operator - (PLfloat a) const {
        return PLVector3D(x - a, y - a, z - a);
    }

    PLVector3D PL_INLINE operator - () const {
        return PLVector3D(-x, -y, -z);
    }

    PLVector3D PL_INLINE operator + (PLVector3D a) const {
        return PLVector3D(x + a.x, y + a.y, z + a.z);
    }

    PLVector3D PL_INLINE operator + (PLfloat a) const {
        return PLVector3D(x + a, y + a, z + a);
    }

    PLVector3D PL_INLINE operator / (const PLVector3D &a) const {
        return PLVector3D(x / a.x, y / a.y, z / a.z);
    }

    PLVector3D PL_INLINE operator / (PLfloat a) const {
        return PLVector3D(x / a, y / a, z / a);
    }

    PL_INLINE PLfloat& operator [] (const unsigned int i) {
        return *((&x) + i);
    }

    bool PL_INLINE operator > (const PLVector3D &v) const {
        return ((x > v.x) && (y > v.y) && (z > v.z));
    }

    bool PL_INLINE operator < (const PLVector3D &v) const {
        return ((x < v.x) && (y < v.y) && (z < v.z));
    }

    bool PL_INLINE operator >= (const PLVector3D &v) const {
        return ((x >= v.x) && (y >= v.y) && (z >= v.z));
    }

    bool PL_INLINE operator <= (const PLVector3D &v) const {
        return ((x <= v.x) && (y <= v.y) && (z <= v.z));
    }

    PLfloat PL_INLINE Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    PLfloat PL_INLINE DotProduct(PLVector3D a) const {
        return (x * a.x + y * a.y + z * a.z);
    }

    PLVector3D PL_INLINE CrossProduct(PLVector3D a) const {
        return PLVector3D(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
    }

    PLVector3D PL_INLINE Normalize() const {
        return (*this) / Length();
    }

    PLfloat PL_INLINE Difference(PLVector3D v) const {
        return ((*this) - v).Length();
    }

    void PL_INLINE Set(PLfloat _x, PLfloat _y, PLfloat _z) {
        x = _x; y = _y; z = _z;
    }

#endif
} PLVector3D;

static PL_INLINE PLVector3D plCreateVector3D(PLfloat x, PLfloat y, PLfloat z) {
    PLVector3D v = { x, y, z };
    return v;
}

static PL_INLINE void plAddVector3D(PLVector3D *v, PLVector3D v2) {
    v->x += v2.x; v->y += v2.y; v->z += v2.z;
}

static PL_INLINE void plSubtractVector3D(PLVector3D *v, PLVector3D v2) {
    v->x -= v2.x; v->y -= v2.y; v->z -= v2.z;
}

static PL_INLINE void plScaleVector3D(PLVector3D *v, PLVector3D v2) {
    v->x *= v2.x; v->y *= v2.y; v->z *= v2.z;
}

static PL_INLINE void plScaleVector3Df(PLVector3D *v, PLfloat f) {
    v->x *= f; v->y *= f; v->z *= f;
}

static PL_INLINE void plDivideVector3D(PLVector3D *v, PLVector3D v2) {
    v->x /= v2.x; v->y /= v2.y; v->z /= v2.z;
}

static PL_INLINE void plInverseVector3D(PLVector3D *v) {
    v->x = -v->x; v->y = -v->y; v->z = -v->z;
}

static PL_INLINE void plClearVector3D(PLVector3D *v) {
    memset(v, 0, sizeof(PLVector3D));
}

PL_INLINE static PLbool plCompareVector3D(PLVector3D v, PLVector3D v2) {
    return ((v.x == v2.x) && (v.y == v2.y) && (v.z == v2.z));
}

static PL_INLINE PLVector3D plVector3DCrossProduct(PLVector3D v, PLVector3D v2) {
      return plCreateVector3D(
              v.y * v2.z - v.z * v2.y,
              v.z * v2.x - v.x * v2.z,
              v.x * v2.y - v.y * v2.x
      );
}

static PL_INLINE PLfloat plVector3DDotProduct(PLVector3D v, PLVector3D v2) {
    return (v.x * v2.x + v.y * v2.y + v.z * v2.z);
}

static PL_INLINE PLfloat plVector3DLength(PLVector3D v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

static PL_INLINE void plNormalizeVector3D(PLVector3D *v) {
    PLfloat length = plVector3DLength(*v);
    if(length != 0) {
        v->x /= length; v->y /= length; v->z /= length;
    }
}

const static PL_INLINE PLchar *plPrintVector3D(PLVector3D v) {
    static PLchar s[32] = { 0 };
    snprintf(s, 32, "%i %i %i", (PLint)v.x, (PLint)v.y, (PLint)v.z);
    return s;
}

#define PL_ORIGIN   plCreateVector3D(0, 0, 0)

// Colour

#define PL_COLOUR_WHITE 255, 255, 255, 255
#define PL_COLOUR_BLACK 0, 0, 0, 255
#define PL_COLOUR_RED   255, 0, 0, 255
#define PL_COLOUR_GREEN 0, 255, 0, 255
#define PL_COLOUR_BLUE  0, 0, 255, 255

typedef struct PLColour {
    PLbyte r, g, b, a;

#ifdef __cplusplus

    PLColour() : PLColour(PL_COLOUR_WHITE) {

    }

    PLColour(PLbyte c, PLbyte c2, PLbyte c3, PLbyte c4 = 255) : r(c), g(c2), b(c3), a(c4) {

    }

    PLColour(PLint c, PLint c2, PLint c3, PLint c4 = 255) :
            PLColour((PLbyte) c, (PLbyte) c2, (PLbyte) c3, (PLbyte) c4) {

    }

    PLColour(PLfloat c, PLfloat c2, PLfloat c3, PLfloat c4 = 1) :
    r(plFloatToByte(c)),
    g(plFloatToByte(c2)),
    b(plFloatToByte(c3)),
    a(plFloatToByte(c4)) {

    }

    PL_INLINE void operator *= (const PLColour &v) {
        r *= v.r; g *= v.g; b *= v.b; a *= v.a;
    }

    PL_INLINE void operator *= (PLfloat c) {
        r *= plFloatToByte(c); g *= plFloatToByte(c); b *= plFloatToByte(c); a *= plFloatToByte(c);
    }

    PL_INLINE void operator *= (PLbyte c) {
        r *= c; g *= c; g *= c; a *= c;
    }

    PL_INLINE void operator += (PLColour v) {
        r += v.r; g += v.g; b += v.b; a += v.a;
    }

    PL_INLINE void operator += (PLfloat c) {
        r += plFloatToByte(c); g += plFloatToByte(c); b += plFloatToByte(c); a += plFloatToByte(c);
    }

    PL_INLINE void operator /= (PLColour v) {
        r /= v.r; g /= v.g; b /= v.b; a /= v.a;
    }

    PL_INLINE void operator /= (PLfloat c) {
        r /= c; g /= c; b /= c; a /= c;
    }

    PL_INLINE PLColour operator - (PLColour c) const {
        return PLColour(r - c.r, g - c.g, b - c.b, a - c.a);
    }

    PL_INLINE PLColour operator - (PLfloat c) const {
        return PLColour(r - plFloatToByte(c), g - plFloatToByte(c), b - plFloatToByte(c), a - plFloatToByte(c));
    }

    PL_INLINE PLColour operator - (PLbyte c) const {
        return PLColour(r - c, g - c, b - c, a - c);
    }

    PL_INLINE PLColour operator - () const {
        return PLColour(-r, -g, -b, -a);
    }

    PL_INLINE PLColour operator * (PLColour v) const {
        return PLColour(r * v.r, g * v.g, b * v.b, a * v.a);
    }

    PL_INLINE PLColour operator + (PLColour v) const {
        return PLColour(r + v.r, g + v.g, b + v.b, a + v.a);
    }

    PL_INLINE PLColour operator + (PLfloat c) const {
        return PLColour(r + plFloatToByte(c), g + plFloatToByte(c), b + plFloatToByte(c), a + plFloatToByte(c));
    }

    PL_INLINE PLColour operator / (const PLColour &v) const {
        return PLColour(r / v.r, g / v.g, b / v.b, a / v.a);
    }

    PL_INLINE PLColour operator / (PLfloat c) const {
        return PLColour(r / plFloatToByte(c), g / plFloatToByte(c), b / plFloatToByte(c), a / plFloatToByte(c));
    }

    PL_INLINE PLColour operator / (PLbyte c) const {
        return PLColour(r / c, g / c, b / c, a / c);
    }

    PL_INLINE PLbyte& operator [] (const unsigned int i) {
        return *((&r) + i);
    }

    PL_INLINE bool operator > (const PLColour &v) const {
        return ((r > v.r) && (g > v.g) && (b > v.b) && (a > v.a));
    }

    PL_INLINE bool operator < (const PLColour &v) const {
        return ((r < v.r) && (g < v.g) && (b < v.b) && (a < v.a));
    }

    PL_INLINE bool operator >= (const PLColour &v) const {
        return ((r >= v.r) && (g >= v.g) && (b >= v.b) && (a >= v.a));
    }

    PL_INLINE bool operator <= (const PLColour &v) const {
        return ((r <= v.r) && (g <= v.g) && (b <= v.b) && (a <= v.a));
    }

#endif
} PLColour;

static PL_INLINE PLColour plCreateColour4b(PLbyte r, PLbyte g, PLbyte b, PLbyte a) {
    PLColour c = { r, g, b, a };
    return c;
}

static PL_INLINE PLColour plCreateColour4f(PLfloat r, PLfloat g, PLfloat b, PLfloat a) {
    PLColour c = {
        plFloatToByte(r),
        plFloatToByte(g),
        plFloatToByte(b),
        plFloatToByte(a)
    };
    return c;
}

static PL_INLINE void plClearColour(PLColour *c) {
    memset(c, 0, sizeof(PLColour));
}

static PL_INLINE void plSetColour4b(PLColour *c, PLbyte r, PLbyte g, PLbyte b, PLbyte a) {
    c->r = r; c->g = g; c->b = b; c->a = a;
}

static PL_INLINE void plSetColour4f(PLColour *c, PLfloat r, PLfloat g, PLfloat b, PLfloat a) {
    c->r = plFloatToByte(r);
    c->g = plFloatToByte(g);
    c->b = plFloatToByte(b);
    c->a = plFloatToByte(a);
}

static PL_INLINE PLbool plCompareColour(PLColour c, PLColour c2) {
    return ((c.r == c2.r) && (c.g == c2.g) && (c.b == c2.b) && (c.a == c2.a));
}

static PL_INLINE void plCopyColour(PLColour *c, PLColour c2) {
    c->r = c2.r; c->g = c2.g; c->b = c2.b; c->a = c2.a;
}

static PL_INLINE void plMultiplyColour(PLColour *c, PLColour c2) {
    c->r *= c2.r; c->g *= c2.g; c->b *= c2.b; c->a *= c2.a;
}

static PL_INLINE void plMultiplyColourf(PLColour *c, PLfloat a) {
    PLbyte a2 = plFloatToByte(a);
    c->r *= a2; c->g *= a2; c->b *= a2; c->a *= a2;
}

static PL_INLINE void plDivideColour(PLColour *c, PLColour c2) {
    c->r /= c2.r; c->g /= c2.g; c->b /= c2.b; c->a /= c2.a;
}

static PL_INLINE void plDivideColourf(PLColour *c, PLfloat a) {
    PLbyte a2 = plFloatToByte(a);
    c->r /= a2; c->g /= a2; c->b /= a2; c->a /= a2;
}

static PL_INLINE const PLchar *plPrintColour(PLColour c) {
    static PLchar s[16] = {0};
    snprintf(s, 16, "%i %i %i %i", c.r, c.g, c.b, c.a);
    return s;
}

// Matrices

typedef PLfloat PLMatrix3[3][3], PLMatrix4[4][4];

static PL_INLINE void plClearMatrix4(PLMatrix4 m) {
    memset(m, 0, sizeof(m[0][0]) * 8);
}

static PL_INLINE void plAddMatrix4(PLMatrix4 m, const PLMatrix4 m2) {
    for(PLint i = 0; i < 4; i++) {
        for(PLint j = 0; j < 4; j++) {
            m[i][j] += m2[i][j];
        }
    }
}

static PL_INLINE void plMultiplyMatrix4(PLMatrix4 m, const PLMatrix4 m2) {
    for(PLint i = 0; i < 4; i++) {
        for(PLint j = 0; j < 4; j++) {
            m[i][j] *= m2[i][j];
        }
    }
}

static PL_INLINE void plMultiplyMatrix4f(PLMatrix4 m, PLfloat a) {
    for(PLint i = 0; i < 4; i++) {
        for(PLint j = 0; j < 4; j++) {
            m[i][j] *= a;
        }
    }
}

static PL_INLINE void plDivisionMatrix4(PLMatrix4 m, const PLMatrix4 m2) {
    for(PLint i = 0; i < 4; i++) {
        for(PLint j = 0; j < 4; j++) {
            m[i][j] /= m2[i][j];
        }
    }
}

static PL_INLINE void plDivisionMatrix4f(PLMatrix4 m, PLfloat a) {
    for(PLint i = 0; i < 4; i++) {
        for(PLint j = 0; j < 4; j++) {
            m[i][j] /= a;
        }
    }
}

static PL_INLINE const PLchar *plPrintMatrix4(const PLMatrix4 m) {
    static PLchar s[256] = {0};
    snprintf(s, 256,
            "%i %i %i %i\n"
            "%i %i %i %i\n"
            "%i %i %i %i\n"
            "%i %i %i %i",
            (PLint)m[0][0], (PLint)m[0][1], (PLint)m[0][2], (PLint)m[0][3],
            (PLint)m[1][0], (PLint)m[1][1], (PLint)m[1][2], (PLint)m[1][3],
            (PLint)m[2][0], (PLint)m[2][1], (PLint)m[2][2], (PLint)m[2][3],
            (PLint)m[3][0], (PLint)m[3][1], (PLint)m[3][2], (PLint)m[3][3]
            );
    return s;
}

// Quaternion

#if 0

typedef PLfloat PLQuaternion[4];

static PL_INLINE void plClearQuaternion(PLQuaternion q) {
    memset(q, 0, sizeof(PLQuaternion));
}

static PL_INLINE void plMultiplyQuaternion(PLQuaternion q, const PLQuaternion q2) {
    q[0] *= q2[0]; q[1] *= q2[1]; q[2] *= q2[2]; q[3] *= q2[3];
}

static PL_INLINE void plMultiplyQuaternionf(PLQuaternion q, PLfloat a) {
    q[0] *= a; q[1] *= a; q[2] *= a; q[3] *= a;
}

static PL_INLINE void plAddQuaternion(PLQuaternion q, const PLQuaternion q2) {
    q[0] += q2[0]; q[1] += q2[1]; q[2] += q2[2]; q[3] += q2[3];
}

static PL_INLINE void plAddQuaternionf(PLQuaternion q, PLfloat a) {
    q[0] += a; q[1] += a; q[2] += a; q[3] += a;
}

static PL_INLINE void plInverseQuaternion(PLQuaternion q) {
    q[0] = -q[0]; q[1] = -q[1]; q[2] = -q[2]; q[3] = -q[3];
}

static PL_INLINE PLfloat plQuaternionLength(PLQuaternion q) {
    return sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
}

static PL_INLINE const PLchar *plPrintQuaternion(const PLQuaternion q) {
    static PLchar s[32] = {0};
    snprintf(s, 32, "%i %i %i %i", (PLint)q[0], (PLint)q[1], (PLint)q[2], (PLint)q[3]);
    return s;
}

#endif

typedef struct PLQuaternion {
    PLfloat x, y, z, w;

#ifdef __cplusplus

    PLQuaternion(PLfloat a, PLfloat b, PLfloat c, PLfloat d) : x(a), y(b), z(c), w(d) {}

    PLQuaternion(PLfloat a, PLfloat b, PLfloat c) : x(a), y(b), z(c), w(0) {}

    PLQuaternion() : x(0), y(0), z(0), w(0) {}

    PL_INLINE void operator = (PLQuaternion a) {
        x = a.x; y = a.y; z = a.z; w = a.w;
    }

    PL_INLINE void operator *= (PLfloat a) {
        x *= a; y *= a; z *= a; w *= a;
    }

    PL_INLINE void operator *= (PLQuaternion a) {
        x *= a.x;
        y *= a.y;
        z *= a.z;
        w *= a.w;
    }

    PL_INLINE PLbool operator == (PLQuaternion a) const {
        return ((x == a.x) && (y == a.y) && (z == a.z) && (w == a.w));
    }

    PL_INLINE PLQuaternion operator * (PLfloat a) {
        return PLQuaternion(x * a, y * a, z * a, w * a);
    }

    PL_INLINE PLQuaternion operator * (PLQuaternion a) {
        return PLQuaternion(x * a.x, y * a.y, z * a.z, w * a.w);
    }

    PL_INLINE void Set(PLfloat a, PLfloat b, PLfloat c, PLfloat d) {
        x = a; y = b; z = c; w = d;
    }

    PL_INLINE void Set(PLfloat a, PLfloat b, PLfloat c) {
        x = a;
        y = b;
        z = c;
    }

    PL_INLINE const PLchar *String() {
        static PLchar s[32] = {0};
        snprintf(s, 32, "%i %i %i %i", (PLint) x, (PLint) y, (PLint) z, (PLint) w);
        return s;
    }

    PL_INLINE PLfloat Length() {
        return std::sqrt((x * x + y * y + z * z + w * w));
    }

    PL_INLINE PLQuaternion Scale(PLfloat a) {
        return PLQuaternion(x * a, y * a, z * a, w * a);
    }

    PL_INLINE PLQuaternion Inverse() {
        return PLQuaternion(-x, -y, -z, w);
    }

#if 0
    PL_INLINE PLQuaternion Normalize() {
        PLfloat l = Length();
        if (l) {
            float i = 1 / l;
            return Scale(i);
        }
    }
#endif

#endif
} PLQuaternion;

// Bounding Boxes

typedef struct PLBBox3D {
    PLVector3D mins, maxs;
} PLBBox3D;

typedef struct PLBBox2D {
    PLVector2D mins, maxs;
} PLBBox2D;

typedef struct PLRectangle {
    PLint x, y;
    PLuint width, height;
} PLRectangle;

// Randomisation

// http://stackoverflow.com/questions/7978759/generate-float-random-values-also-negative
static PL_INLINE PLdouble plUniform0To1Random(void) {
    return (random()) / ((double) RAND_MAX + 1);
}

static PL_INLINE PLdouble plGenerateUniformRandom(double minmax) {
    return (minmax * 2) * plUniform0To1Random() - minmax;
}

static PL_INLINE PLdouble plGenerateRandomd(PLdouble max) {
    return (PLdouble) (rand()) / (RAND_MAX / max);
}

static PL_INLINE PLfloat plGenerateRandomf(PLfloat max) {
    return (PLfloat) (rand()) / (RAND_MAX / max);
}

// Interpolation
// http://paulbourke.net/miscellaneous/interpolation/

static PL_INLINE PLfloat plLinearInterpolate(PLfloat y1, PLfloat y2, PLfloat mu) {
    return (y1 * (1 - mu) + y2 * mu);
}

static PL_INLINE PLfloat plCosineInterpolate(PLfloat y1, PLfloat y2, PLfloat mu) {
    PLfloat mu2 = (1 - cosf(mu * (PLfloat) PL_PI)) / 2;
    return (y1 * (1 - mu2) + y2 * mu2);
}

// http://probesys.blogspot.co.uk/2011/10/useful-math-functions.html

static PL_INLINE PLfloat plOutPow(PLfloat x, PLfloat p) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    PLint sign = (PLint) p % 2 == 0 ? -1 : 1;
    return (sign * (powf(x - 1.0f, p) + sign));
}

static PL_INLINE PLfloat plLinear(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return x;
}

static PL_INLINE PLfloat plInPow(PLfloat x, PLfloat p) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return powf(x, p);
}

static PL_INLINE PLfloat plInSin(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return -cosf(x * ((float) PL_PI / 2.0f)) + 1.0f;
}

static PL_INLINE PLfloat plOutSin(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return sinf(x * ((float) PL_PI / 2.0f));
}

static PL_INLINE PLfloat plInExp(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return powf(2.0f, 10.0f * (x - 1.0f));
}

static PL_INLINE PLfloat plOutExp(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return -powf(2.0f, -1.0f * x) + 1.0f;
}

static PL_INLINE PLfloat plInOutExp(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return x < 0.5f ? 0.5f * powf(2.0f, 10.0f * (2.0f * x - 1.0f)) :
           0.5f * (-powf(2.0f, 10.0f * (-2.0f * x + 1.0f)) + 1.0f);
}

static PL_INLINE PLfloat plInCirc(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return -(sqrtf(1.0f - x * x) - 1.0f);
}

static PL_INLINE PLfloat plOutBack(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return (x - 1.0f) * (x - 1.0f) * ((1.70158f + 1.0f) * (x - 1.0f) + 1.70158f) + 1.0f;
}

// The variable, k, controls the stretching of the function.
static PL_INLINE PLfloat plImpulse(PLfloat x, PLfloat k) {
    PLfloat h = k * x;
    return h * expf(1.0f - h);
}

static PL_INLINE PLfloat plRebound(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    if (x < (1.0f / 2.75f)) {
        return 1.0f - 7.5625f * x * x;
    } else if (x < (2.0f / 2.75f)) {
        return 1.0f - (7.5625f * (x - 1.5f / 2.75f) *
                       (x - 1.5f / 2.75f) + 0.75f);
    } else if (x < (2.5f / 2.75f)) {
        return 1.0f - (7.5625f * (x - 2.25f / 2.75f) *
                       (x - 2.25f / 2.75f) + 0.9375f);
    } else {
        return 1.0f - (7.5625f * (x - 2.625f / 2.75f) * (x - 2.625f / 2.75f) +
                       0.984375f);
    }
}

static PL_INLINE PLfloat plExpPulse(PLfloat x, PLfloat k, PLfloat n) {
    return expf(-k * powf(x, n));
}

static PL_INLINE PLfloat plInOutBack(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return x < 0.5f ? 0.5f * (4.0f * x * x * ((2.5949f + 1.0f) * 2.0f * x - 2.5949f)) :
           0.5f * ((2.0f * x - 2.0f) * (2.0f * x - 2.0f) * ((2.5949f + 1.0f) * (2.0f * x - 2.0f) +
                                                            2.5949f) + 2.0f);
}

static PL_INLINE PLfloat plInBack(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return x * x * ((1.70158f + 1.0f) * x - 1.70158f);
}

static PL_INLINE PLfloat plInOutCirc(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return x < 1.0f ? -0.5f * (sqrtf(1.0f - x * x) - 1.0f) :
           0.5f * (sqrtf(1.0f - ((1.0f * x) - 2.0f) * ((2.0f * x) - 2.0f)) + 1.0f);
}

static PL_INLINE PLfloat plOutCirc(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return sqrtf(1.0f - (x - 1.0f) * (x - 1.0f));
}

static PL_INLINE PLfloat plInOutSin(PLfloat x) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    return -0.5f * (cosf((PLfloat) PL_PI * x) - 1.0f);
}

static PL_INLINE PLfloat plInOutPow(PLfloat x, PLfloat p) {
    if (x < 0) {
        return 0;
    } else if (x > 1.0f) {
        return 1.0f;
    }

    int sign = (int) p % 2 == 0 ? -1 : 1;
    return (sign / 2.0f * (powf(x - 2.0f, p) + sign * 2.0f));
}

//////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS

#if defined(PL_INTERNAL)

void _plDebugMath(void);

#endif
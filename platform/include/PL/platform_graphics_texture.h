
#pragma once

typedef struct PLImage PLImage;

// Texture Environment Modes
typedef enum PLTextureEnvironmentMode {
    PL_TEXTUREMODE_ADD,
    PL_TEXTUREMODE_MODULATE,
    PL_TEXTUREMODE_DECAL,
    PL_TEXTUREMODE_BLEND,
    PL_TEXTUREMODE_REPLACE,
    PL_TEXTUREMODE_COMBINE
} PLTextureEnvironmentMode;

typedef enum PLTextureClamp {
#if defined (PL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    VL_TEXTURECLAMP_CLAMP = GL_CLAMP_TO_EDGE,
    VL_TEXTURECLAMP_WRAP = GL_REPEAT,
#elif defined (VL_MODE_GLIDE)
    VL_TEXTURECLAMP_CLAMP	= GR_TEXTURECLAMP_CLAMP,
    VL_TEXTURECLAMP_WRAP	= GR_TEXTURECLAMP_WRAP,
#else
    VL_TEXTURECLAMP_CLAMP,
    VL_TEXTURECLAMP_WRAP,
#endif
} PLTextureClamp;

typedef enum PLTextureFilter {
    PL_TEXTUREFILTER_MIPMAP_NEAREST,        // GL_NEAREST_MIPMAP_NEAREST
    PL_TEXTUREFILTER_MIPMAP_LINEAR,         // GL_LINEAR_MIPMAP_LINEAR

    PL_TEXTUREFILTER_MIPMAP_LINEAR_NEAREST,    // GL_LINEAR_MIPMAP_NEAREST
    PL_TEXTUREFILTER_MIPMAP_NEAREST_LINEAR,    // GL_NEAREST_MIPMAP_LINEAR

    PL_TEXTUREFILTER_NEAREST,       // Nearest filtering
    PL_TEXTUREFILTER_LINEAR         // Linear filtering
} PLTextureFilter;

typedef enum PLTextureTarget {
    PL_TEXTURE_1D,
    PL_TEXTURE_2D,
    PL_TEXTURE_3D
} PLTextureTarget;

enum PLTextureFlag {
    PL_TEXTUREFLAG_PRESERVE = (1 << 0)
};

typedef struct PLTextureMappingUnit {
    PLbool active;

    PLuint current_texture;
    PLuint current_capabilities;

    PLTextureEnvironmentMode current_envmode;
} PLTextureMappingUnit;

#ifdef __cplusplus

typedef struct PLTexture PLTexture;

extern "C" {

PL_EXTERN void plBindTexture(PLTexture *texture);

}

#endif

typedef struct PLTexture {
    PLuint id;

    PLuint flags;

    PLuint x, y;
    PLuint width, height;

    PLuint size;
    PLuint levels;
    PLuint crc;

    const PLchar *path;

    PLImageFormat format;
    PLDataFormat storage;
    PLColourFormat pixel;

#ifdef __cplusplus

    PL_INLINE void Bind() {
        plBindTexture(this);
    }

    PL_INLINE PLuint GetFlags() const {
        return flags;
    }

    PL_INLINE void AddFlags(PLuint f) {
        flags |= f;
    }

    PL_INLINE void SetFlags(PLuint f) {
        flags = f;
    }

    PL_INLINE void RemoveFlags(PLuint f) {
        flags &= ~f;
    }

    PL_INLINE void ClearFlags() {
        flags = 0;
    }

#endif
} PLTexture;

PL_EXTERN_C

PL_EXTERN PLTexture *plCreateTexture(void);
PL_EXTERN void plDeleteTexture(PLTexture *texture, PLbool force);

//PL_EXTERN PLresult plUploadTextureData(PLTexture *texture, const PLTextureInfo *upload);
PL_EXTERN PLresult plUploadTextureImage(PLTexture *texture, const PLImage *upload);

PL_EXTERN PLuint plGetMaxTextureSize(void);
PL_EXTERN PLuint plGetMaxTextureUnits(void);
PL_EXTERN PLuint plGetMaxTextureAnistropy(void);

PL_EXTERN PLresult plSetTextureFilter(PLTexture *texture, PLTextureFilter filter);
PL_EXTERN PLresult plSetTextureAnisotropy(PLTexture *texture, PLuint amount);

PL_EXTERN void plSetTextureUnit(PLuint target);
PL_EXTERN void plSetTextureEnvironmentMode(PLTextureEnvironmentMode mode);

PL_EXTERN const PLchar *plPrintTextureMemoryUsage(void);

#if defined(PL_INTERNAL)

PL_EXTERN unsigned int _plTranslateColourFormat(PLColourFormat format);
PL_EXTERN unsigned int _plTranslateTextureFormat(PLImageFormat format);

#endif

PL_EXTERN_C_END
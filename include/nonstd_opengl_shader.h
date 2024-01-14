#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include "nonstd.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define XLAYOUTS                            \
    X(aPosition, 0, 3, mVertices)           \
    X(aNormal, 1, 3, mNormals)              \
    X(aTangent, 2, 3, mTangents)            \
    X(aBitTangent, 3, 3, mBitangents)       \
    X(aColor0, 4, 3, mColors[0])            \
    X(aColor1, 5, 3, mColors[1])            \
    X(aColor2, 6, 3, mColors[2])            \
    X(aColor3, 7, 3, mColors[3])            \
    X(aColor4, 8, 3, mColors[4])            \
    X(aColor5, 9, 3, mColors[5])            \
    X(aColor6, 10, 3, mColors[6])           \
    X(aColor7, 11, 3, mColors[7])           \
    X(aTexCoord0, 12, 3, mTextureCoords[0]) \
    X(aTexCoord1, 13, 3, mTextureCoords[1]) \
    X(aTexCoord2, 14, 3, mTextureCoords[2]) \
    X(aTexCoord3, 15, 3, mTextureCoords[3]) \
    X(aTexCoord4, 16, 3, mTextureCoords[4]) \
    X(aTexCoord5, 17, 3, mTextureCoords[5]) \
    X(aTexCoord6, 18, 3, mTextureCoords[6]) \
    X(aTexCoord7, 19, 3, mTextureCoords[7])

#define NUM_LAYOUT_POSITIONS 20

#define XVS_TO_FS \
    X(vec2, TexCoord0)

#define XMATERIALS      \
    X(None)             \
    X(Diffuse)          \
    X(Specular)         \
    X(Ambient)          \
    X(Emissive)         \
    X(Height)           \
    X(Normals)          \
    X(Shininess)        \
    X(Opacity)          \
    X(Displacemet)      \
    X(Lightmap)         \
    X(Reflection)       \
    X(BaseColor)        \
    X(NormalCamera)     \
    X(EmissiveColor)    \
    X(Metalness)        \
    X(DiffuseRoughness) \
    X(AmbientOcclusion) \
    X(Sheen)            \
    X(ClearCoat)        \
    X(Transmission)

    typedef enum shader_set_type_e
    {
        F1,
        F2,
        F3,
        F4,
        I1,
        I2,
        I3,
        I4,
        UI1,
        UI2,
        UI3,
        UI4,
        M2,
        M3,
        M4
    } shader_set_type_t;

    typedef struct shader_attrubute_s
    {
        unsigned int mAttributeType;
        int mAttributeSize;

    } shader_attrubute_t;

    typedef struct nonstd_opengl_shader_s
    {
        unsigned int ID;
    } shader_t;

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    int shader_init(shader_t *shader, const char *vertexPath, const char *fragmentPath);
    int shader_free(shader_t *shader);
    // activate the shader
    // ------------------------------------------------------------------------
    int shader_use(const shader_t *shader);
    int shader_bindBuffer(const shader_t *shader, const char *name, const unsigned int index);

    int shader_set(const shader_t *shader, const char *name, const shader_set_type_t type, const int count, void *value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
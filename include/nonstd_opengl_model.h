#ifndef OPENGL_MODEL_H
#define OPENGL_MODEL_H

#include <assimp/scene.h>
#include <cglm/cglm.h>

typedef struct material_s
{
    unsigned int mTextureCount[AI_TEXTURE_TYPE_MAX + 1];
    unsigned long int *mTextureIndex[AI_TEXTURE_TYPE_MAX + 1];
    unsigned int mNumMeshes;
    unsigned int *mMeshes;
} material_t;

typedef struct mesh_s
{
    unsigned int mMaterialIndex;
    unsigned int mNumElements;

    unsigned int mVAO;

    unsigned int mElementBuffer;
    unsigned int mVertexBuffer;
    unsigned int mNormalBuffer;
    unsigned int mTangentBuffer;
    unsigned int mBitTangentBuffer;
    unsigned int mColorBuffers[AI_MAX_NUMBER_OF_COLOR_SETS];
    unsigned int mTexCoordBuffers[AI_MAX_NUMBER_OF_TEXTURECOORDS];

    unsigned int mNumInstances;
    mat4 *mTransformation;

} mesh_t;

typedef struct node_mesh_data_s
{
    unsigned int mMeshInstance;
    unsigned int mMeshIndex;
}node_mesh_data_t;

typedef struct model_node_s
{
    mat4 mTransformation;
    struct model_node_s *mParent;
    unsigned int mNumChildren;
    struct model_node_s *mChildren;
    unsigned int mNumMeshes;
    node_mesh_data_t *mMeshData;

} model_node_t;

typedef struct model_s
{
    const shader_t *mShader;
    const char *mModelPath;
    int mPathLength;

    unsigned int mNumMaterials;
    material_t *mMaterialList;

    unsigned int mNumMeshes;
    mesh_t *mMeshList;

    model_node_t mRootNode;

} model_t;

int model_alloc(model_t *model, shader_t *shader, const char *rootpath, const char *model_path, const int path_length);
int model_free(model_t *model);


int model_draw(model_t *model);

#endif
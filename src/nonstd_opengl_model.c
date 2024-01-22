#define MAX_PATH_LENGTH 1026

/*
TODO glMultiDrawElementsIndirect
*/

#include <stdio.h>

#include <GL/glew.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cglm/cglm.h>

#include "nonstd.h"
#include "nonstd_opengl_shader.h"
#include "nonstd_opengl_texture.h"
#include "nonstd_opengl_model.h"

#define MODEL_LOADING_FLAGS aiProcessPreset_TargetRealtime_MaxQuality

static inline __attribute((always_inline)) void aiMat4ToCglmMat4(struct aiMatrix4x4 mat, mat4 dest)
{
    memcpy(dest, &mat, sizeof(mat4));
}

#define X(N) #N, 
const char *materialNames[] = {
    XMATERIALS
};
#undef X

int material_alloc(material_t *material, const char *rootpath, const struct aiMaterial *aiMaterial)
{

    for (int type = aiTextureType_NONE; type < AI_TEXTURE_TYPE_MAX + 1; type++)
    {
        unsigned int TextureCount = aiGetMaterialTextureCount(aiMaterial, type);

        material->mTextureCount[type] = TextureCount;

        if (TextureCount > 0)
        {
            CHECK(safe_alloc((void **)&(material->mTextures[type]), TextureCount * sizeof(material_texture_t)), return retval);
            memset((void *)material->mTextures[type], 0, TextureCount * sizeof(material_texture_t));
        }

        for (unsigned int index = 0; index < TextureCount; index++)
        {
            char file_path[MAX_PATH_LENGTH];
            memset(file_path, 0, MAX_PATH_LENGTH);
            material_texture_t *texture_data = &(material->mTextures[type][index]);

            aiGetMaterialTexture(
                aiMaterial,
                type,
                index,
                &(texture_data->path),
                &(texture_data->mapping),
                &(texture_data->uvindex),
                &(texture_data->blend),
                &(texture_data->op),
                &(texture_data->mapmode),
                &(texture_data->flags));

            int path_length = snprintf(file_path, MAX_PATH_LENGTH - 1, "%s/%s", rootpath, texture_data->path.data);
            if (path_length > MAX_PATH_LENGTH)
                path_length = MAX_PATH_LENGTH;

            texture_data->mTextureIndex = (unsigned long)-1l;
            CHECK(get_load_texture(&(texture_data->mTextureIndex), file_path, path_length), return retval);
            snprintf((char *)texture_data->name, 1023, "Material.%s[%d]", materialNames[type], index);
        }
    }
    return 0;
}

int material_free(material_t *material)
{
    for (int type = aiTextureType_NONE; type < AI_TEXTURE_TYPE_MAX + 1; type++)
    {
        if (material->mTextureCount[type] > 0)
        {
            CHECK(safe_free((void **)&(material->mTextures[type]), material->mTextureCount[type] * sizeof(material_texture_t)), return retval);
        }
    }
    return 0;
}

int mesh_alloc(mesh_t *mesh, struct aiMesh *aiMesh)
{
    if (aiMesh->mNumFaces <= 0)
    {
        return 0;
    }
    mesh->mMaterialIndex = aiMesh->mMaterialIndex;

    mesh->mVAO = GL_FALSE;
    glGenVertexArrays(1, &(mesh->mVAO));
    glBindVertexArray(mesh->mVAO);

    // Buffers
    {
        unsigned int elementsPerFace = 0;
        switch (aiMesh->mPrimitiveTypes)
        {
        case aiPrimitiveType_POINT:
            elementsPerFace = 1;
            break;
        case aiPrimitiveType_LINE:
            elementsPerFace = 2;
            break;
        case aiPrimitiveType_TRIANGLE:
        case aiPrimitiveType_NGONEncodingFlag:
            elementsPerFace = 3;
            break;
        case aiPrimitiveType_POLYGON:
            break;
        default:
            return 0;
            break;
        }

        if (aiMesh->mNumFaces > 0)
        {
            if (elementsPerFace > 0)
            {
                mesh->mNumElements = aiMesh->mNumFaces * elementsPerFace;
            }
            else
            {
                for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
                {
                    mesh->mNumElements += aiMesh->mFaces[i].mNumIndices;
                }
            }
        }

        glGenBuffers(sizeof(mesh->mBuffers) / sizeof(unsigned int), (unsigned int *)&(mesh->mBuffers));

        if (mesh->mNumElements > 0 && aiMesh->mFaces != NULL)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mBuffers.mElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->mNumElements * sizeof(unsigned int), NULL, GL_STATIC_DRAW);
            unsigned int offset = 0;

            for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
            {
                unsigned int face_size = aiMesh->mFaces[i].mNumIndices * sizeof(unsigned int);
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, face_size, aiMesh->mFaces[i].mIndices);
                offset += face_size;
            }
        }

        {
#define X(L, I, S, N)                                  \
    do                                                 \
    {                                                  \
        if (aiMesh->N != NULL)                       \
        {                                              \
            glBindBuffer(                              \
                GL_ARRAY_BUFFER,                       \
                mesh->mBuffers.L);                   \
            glBufferData(                              \
                GL_ARRAY_BUFFER,                       \
                aiMesh->mNumVertices * sizeof(vec##S), \
                aiMesh->N,                           \
                GL_STATIC_DRAW);                       \
            glEnableVertexAttribArray(I);              \
            glVertexAttribPointer(                     \
                I,                                     \
                S,                                     \
                GL_FLOAT,                              \
                GL_FALSE,                              \
                sizeof(vec##S),                        \
                (void *)0);                            \
        }                                              \
    } while (0);
            XLAYOUTS
#undef X
        }
    }

    glBindVertexArray(GL_FALSE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_FALSE);
    glBindBuffer(GL_ARRAY_BUFFER, GL_FALSE);

    return 0;
}

int mesh_free(mesh_t *mesh)
{

    glDeleteBuffers(sizeof(mesh->mBuffers) / sizeof(unsigned int), (unsigned int *)&(mesh->mBuffers));

    glDeleteVertexArrays(1, &(mesh->mVAO));

    return 0;
}

int model_node_alloc(model_node_t *node, model_node_t *parent_node, model_t *model, struct aiNode *aiNode)
{
    aiMat4ToCglmMat4(aiNode->mTransformation, node->mTransformation);
    node->mParent = parent_node;
    node->mNumMeshes = aiNode->mNumMeshes;
    if (node->mNumMeshes > 0)
    {
        CHECK(safe_alloc((void **)&(node->mMeshData), node->mNumMeshes * sizeof(node_mesh_data_t)), return retval);
        memset(node->mMeshData, 0, node->mNumMeshes * sizeof(node_mesh_data_t));
    }
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        node->mMeshData[i].mMeshIndex = aiNode->mMeshes[i];
        node->mMeshData[i].mMeshInstance = model->mMeshList[aiNode->mMeshes[i]].mNumInstances++;
    }

    node->mNumChildren = aiNode->mNumChildren;
    if (node->mNumChildren > 0)
    {
        CHECK(safe_alloc((void **)&(node->mChildren), node->mNumChildren * sizeof(model_node_t)), return retval);
        memset(node->mChildren, 0, node->mNumChildren * sizeof(model_node_t));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        model_node_alloc(&(node->mChildren[i]), node, model, aiNode->mChildren[i]);
    }

    return 0;
}

int model_node_free(model_node_t *node)
{

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        model_node_free(&(node->mChildren[i]));
    }
    if (node->mNumMeshes > 0)
    {
        CHECK(safe_free((void **)&(node->mMeshData), node->mNumMeshes * sizeof(node_mesh_data_t)), return retval);
    }
    if (node->mNumChildren > 0)
    {
        CHECK(safe_free((void **)&(node->mChildren), node->mNumChildren * sizeof(model_node_t)), return retval);
    }
    return 0;
}

int model_node_update_mesh_transforms(model_node_t *node, model_t *model, mat4 transform)
{
    mat4 node_transform;
    glm_mat4_mul(node->mTransformation, transform, node_transform);
    for (unsigned int index = 0; index < node->mNumMeshes; index++)
    {
        node_mesh_data_t *model_node_s = &(node->mMeshData[index]);
        mat4 *mesh_transform = &(model->mMeshList[model_node_s->mMeshIndex].mTransformation[model_node_s->mMeshInstance]);
        glm_mat4_copy(node_transform, *mesh_transform);
    }
    for (unsigned int index = 0; index < node->mNumChildren; index++)
    {
        model_node_update_mesh_transforms(&(node->mChildren[index]), model, node_transform);
    }
    return 0;
}

int model_alloc(model_t *model, shader_t *shader, const char *rootpath, const char *model_name, const int path_length)
{
    model->mShader = shader;
    model->mModelName = model_name;
    model->mPathLength = path_length;
    // Start the import on the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll t
    // probably to request more postprocessing than we do in this example.
    char file_path[MAX_PATH_LENGTH];
    memset(file_path, 0, MAX_PATH_LENGTH);
    snprintf(file_path, MAX_PATH_LENGTH - 1, "%s/%s", rootpath, model_name);
    const struct aiScene *scene = aiImportFile(file_path, MODEL_LOADING_FLAGS);

    // If the import failed, report it
#ifdef ERROR_CHECKING
    THROW_ERR((NULL == scene), aiGetErrorString(), return retval);
#endif

    // THROW_ERR((scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE), "INCOMPLETE SCENE", return retval);
    THROW_ERR(!(scene->mFlags && AI_SCENE_FLAGS_VALIDATED), "INVALID SCENE", return retval);
    // THROW_ERR((scene->mFlags && AI_SCENE_FLAGS_VALIDATION_WARNING), "INVALID SCENE", return retval);

    // Now we can access the file's contents
    // DoTheSceneProcessing(scene);
    THROW_ERR((scene->mNumMaterials == 0), "NO MATERIALS IN SCENE", return retval);

    model->mNumMaterials = scene->mNumMaterials;
    CHECK(safe_alloc((void **)&(model->mMaterialList), model->mNumMaterials * sizeof(material_t)), return retval);
    memset((void *)model->mMaterialList, 0, model->mNumMaterials * sizeof(material_t));

    for (unsigned int i = 0; i < model->mNumMaterials; i++)
    {
        CHECK(material_alloc(&(model->mMaterialList[i]), rootpath, scene->mMaterials[i]), return retval);

        model->mMaterialList[i].mNumMeshes = 0;
        for (unsigned int j = 0; j < scene->mNumMeshes; j++)
        {
            if (scene->mMeshes[j]->mMaterialIndex == i)
            {
                model->mMaterialList[i].mNumMeshes++;
            }
        }
        CHECK(safe_alloc((void **)&(model->mMaterialList[i].mMeshes), model->mMaterialList[i].mNumMeshes * sizeof(unsigned int)), return retval);
        unsigned int index = 0;
        for (unsigned int j = 0; j < scene->mNumMeshes; j++)
        {
            if (scene->mMeshes[j]->mMaterialIndex == i)
            {
                model->mMaterialList[i].mMeshes[index++] = j;
            }
        }
    }

    model->mNumMeshes = scene->mNumMeshes;
    CHECK(safe_alloc((void **)&(model->mMeshList), scene->mNumMeshes * sizeof(mesh_t)), return retval);
    memset((void *)model->mMeshList, 0, model->mNumMeshes * sizeof(mesh_t));

    for (unsigned int i = 0; i < model->mNumMeshes; i++)
    {
        CHECK(mesh_alloc(&(model->mMeshList[i]), scene->mMeshes[i]), return retval);
    }

    model_node_alloc(&(model->mRootNode), NULL, model, scene->mRootNode);

    for (unsigned int i = 0; i < model->mNumMeshes; i++)
    {
        CHECK(safe_alloc((void **)&(model->mMeshList[i].mTransformation), model->mMeshList[i].mNumInstances * sizeof(mat4)), return retval);
        memset((void *)model->mMeshList[i].mTransformation, 0, model->mMeshList[i].mNumInstances * sizeof(mat4));
    }

    // We're done. Release all resources associated with this import
    aiReleaseImport(scene);

    return 0;
}

int model_free(model_t *model)
{
    for (unsigned int i = 0; i < model->mNumMaterials; i++)
    {
        CHECK(material_free(&(model->mMaterialList[i])), return retval);
    }
    CHECK(safe_free((void **)&(model->mMaterialList), model->mNumMaterials * sizeof(material_t)), return retval);

    for (unsigned int i = 0; i < model->mNumMeshes; i++)
    {
        CHECK(mesh_free(&(model->mMeshList[i])), return retval);
    }
    CHECK(safe_free((void **)&(model->mMeshList), model->mNumMeshes * sizeof(mesh_t)), return retval);

    CHECK(model_node_free(&(model->mRootNode)), return retval);

    model->mModelName = NULL;
    model->mPathLength = 0;
    return 0;
}

int mesh_draw(mesh_t *mesh, const shader_t *shader)
{
    // draw mesh
    CHECK(shader_set(shader, "model", M4, mesh->mNumInstances, mesh->mTransformation), return retval);
    glBindVertexArray(mesh->mVAO);
    glDrawElementsInstanced(GL_TRIANGLES, mesh->mNumElements, GL_UNSIGNED_INT, 0, mesh->mNumInstances);
    glBindVertexArray(0);
    return 0;
}

int material_draw(material_t *material, model_t *model)
{

    for (int type = aiTextureType_NONE; type < AI_TEXTURE_TYPE_MAX + 1; type++)
    {
        for (unsigned int index = 0; index < material->mTextureCount[type]; index++)
        {
            material_texture_t *texture_data = &(material->mTextures[type][index]);

            int unit = -1;
            CHECK(texture_activate(texture_data->mTextureIndex, &unit), return retval);
            CHECK(shader_set(model->mShader, texture_data->name, I1, 1, &unit), return retval);
        }
    }

    for (unsigned int j = 0; j < material->mNumMeshes; j++)
    {
        CHECK(mesh_draw(&(model->mMeshList[material->mMeshes[j]]), model->mShader), return retval);
    }
    return 0;
}

int model_draw(model_t *model, mat4 transform)
{

    model_node_update_mesh_transforms(&(model->mRootNode), model, transform);

    CHECK(shader_use(model->mShader), return retval);
    for (unsigned int i = 0; i < model->mNumMaterials; i++)
    {
        CHECK(material_draw(&(model->mMaterialList[i]), model), return retval);
    }

    return 0;
}
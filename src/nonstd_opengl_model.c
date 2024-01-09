#define AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE 80.0
#define AI_SLM_DEFAULT_MAX_VERTICES 1000000
#define AI_SLM_DEFAULT_MAX_TRIANGLES 1000000
#define AI_LMW_MAX_WEIGHTS 4
#define PP_ICL_PTCACHE_SIZE 12
#define MAX_PATH_LENGTH 1026

#include <stdio.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cglm/cglm.h>

#include "nonstd.h"
#include "nonstd_opengl_shader.h"
#include "nonstd_opengl_texture.h"
#include "nonstd_opengl_model.h"

#define MODEL_LOADING_FLAGS                  \
    aiProcess_CalcTangentSpace |             \
        aiProcess_JoinIdenticalVertices |    \
        aiProcess_Triangulate |              \
        aiProcess_GenSmoothNormals |         \
        aiProcess_SplitLargeMeshes |         \
        aiProcess_LimitBoneWeights |         \
        aiProcess_ValidateDataStructure |    \
        aiProcess_ImproveCacheLocality |     \
        aiProcess_RemoveRedundantMaterials | \
        aiProcess_SortByPType |              \
        aiProcess_FindDegenerates |          \
        aiProcess_FindInvalidData |          \
        aiProcess_GenUVCoords |              \
        aiProcess_TransformUVCoords |        \
        aiProcess_FindInstances |            \
        aiProcess_OptimizeMeshes |           \
        aiProcess_OptimizeGraph |            \
        aiProcess_SplitByBoneCount |         \
        aiProcess_Debone |                   \
        aiProcess_GenBoundingBoxes

static inline __attribute((always_inline)) void aiMat4ToCglmMat4(struct aiMatrix4x4 mat, mat4 dest)
{
    memcpy(dest, &mat, sizeof(mat4));
}

int material_alloc(material_t *material, const char *rootpath, const struct aiMaterial *aiMaterial)
{

    for (int type = aiTextureType_NONE; type < AI_TEXTURE_TYPE_MAX + 1; type++)
    {
        unsigned int TextureCount = aiGetMaterialTextureCount(aiMaterial, type);

        material->mTextureCount[type] = TextureCount;

        if (TextureCount > 0)
        {
            CHECK(safe_alloc((void **)&(material->mTextureIndex[type]), TextureCount * sizeof(unsigned long int)), return retval);
            memset((void *)material->mTextureIndex[type], 0, TextureCount * sizeof(unsigned long int));
        }

        for (unsigned int index = 0; index < TextureCount; index++)
        {
            char file_path[MAX_PATH_LENGTH];
            memset(file_path, 0, MAX_PATH_LENGTH);
            struct aiString path;

            aiGetMaterialTexture(aiMaterial, type, index, &path, NULL, NULL, NULL, NULL, NULL, NULL);

            int path_length = snprintf(file_path, MAX_PATH_LENGTH - 1, "%s/%s", rootpath, path.data);
            if (path_length > MAX_PATH_LENGTH)
                path_length = MAX_PATH_LENGTH;
            // material->mTextureIndex[type][index] = 0;
            material->mTextureIndex[type][index] = (unsigned long)-1l;
            CHECK(get_load_texture(&(material->mTextureIndex[type][index]), file_path, path_length), return retval);
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
            CHECK(safe_free((void **)&(material->mTextureIndex[type]), material->mTextureCount[type] * sizeof(unsigned long int)), return retval);
        }
    }
    return 0;
}

int mesh_alloc(mesh_t *mesh, shader_t *shader, struct aiMesh *aiMesh)
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

        if (mesh->mNumElements > 0 && aiMesh->mFaces != NULL)
        {
            glGenBuffers(1, &(mesh->mElementBuffer));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->mNumElements * sizeof(unsigned int), NULL, GL_STATIC_DRAW);
            unsigned int offset = 0;

            for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
            {
                unsigned int face_size = aiMesh->mFaces[i].mNumIndices * sizeof(unsigned int);
                // for(unsigned int j = 0; j < aiMesh->mFaces[i].mNumIndices; j++)
                // {
                //     fprintf(stderr, "%u ", aiMesh->mFaces[i].mIndices[j]);
                // }
                // fprintf(stderr, "\n");

                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, face_size, aiMesh->mFaces[i].mIndices);
                offset += face_size;
            }
        }

        if (aiMesh->mVertices != NULL)
        {

            // for(unsigned int j = 0; j < aiMesh->mNumVertices; j++)
            // {
            //     fprintf(stderr, "%u: %f,%f,%f\n",j, aiMesh->mVertices[j].x,aiMesh->mVertices[j].y,aiMesh->mVertices[j].z);
            // }

            glGenBuffers(1, &(mesh->mVertexBuffer));
            glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, aiMesh->mNumVertices * sizeof(struct aiVector3D), aiMesh->mVertices, GL_STATIC_DRAW);

            long int index = -1l;
            CHECK(hashmap_find((void **)&index, &(shader->mAttributeMap), (unsigned char *)"aPosition", 9), return retval);

            if (index > -1l)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(
                    index,                     // Attribute Index
                    3,                         // attribute size
                    GL_FLOAT,                  // attribute type
                    GL_FALSE,                  // should be normalized
                    sizeof(struct aiVector3D), // attribute stride
                    (void *)0                  // attribute offset
                );
            }
        }

        if (aiMesh->mNormals != NULL)
        {
            glGenBuffers(1, &(mesh->mNormalBuffer));
            glBindBuffer(GL_ARRAY_BUFFER, mesh->mNormalBuffer);
            glBufferData(GL_ARRAY_BUFFER, aiMesh->mNumVertices * sizeof(struct aiVector3D), aiMesh->mNormals, GL_STATIC_DRAW);

            long int index = -1l;
            CHECK(hashmap_find((void **)&index, &(shader->mAttributeMap), (unsigned char *)"aNormal", 7), return retval);

            if (index > -1l)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(
                    index,                     // Attribute Index
                    3,                         // attribute size
                    GL_FLOAT,                  // attribute type
                    GL_FALSE,                  // should be normalized
                    sizeof(struct aiVector3D), // attribute stride
                    (void *)0                  // attribute offset
                );
            }
        }

        if (aiMesh->mTangents != NULL)
        {
            glGenBuffers(1, &(mesh->mTangentBuffer));
            glBindBuffer(GL_ARRAY_BUFFER, mesh->mTangentBuffer);
            glBufferData(GL_ARRAY_BUFFER, aiMesh->mNumVertices * sizeof(struct aiVector3D), aiMesh->mTangents, GL_STATIC_DRAW);

            long int index = -1l;
            CHECK(hashmap_find((void **)&index, &(shader->mAttributeMap), (unsigned char *)"aTangent", 8), return retval);

            if (index > -1l)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(
                    index,                     // Attribute Index
                    3,                         // attribute size
                    GL_FLOAT,                  // attribute type
                    GL_FALSE,                  // should be normalized
                    sizeof(struct aiVector3D), // attribute stride
                    (void *)0                  // attribute offset
                );
            }
        }

        if (aiMesh->mBitangents != NULL)
        {
            glGenBuffers(1, &(mesh->mBitTangentBuffer));
            glBindBuffer(GL_ARRAY_BUFFER, mesh->mBitTangentBuffer);
            glBufferData(GL_ARRAY_BUFFER, aiMesh->mNumVertices * sizeof(struct aiVector3D), aiMesh->mBitangents, GL_STATIC_DRAW);

            long int index = -1l;
            CHECK(hashmap_find((void **)&index, &(shader->mAttributeMap), (unsigned char *)"aBitTangent", 11), return retval);

            if (index > -1l)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(
                    index,                     // Attribute Index
                    3,                         // attribute size
                    GL_FLOAT,                  // attribute type
                    GL_FALSE,                  // should be normalized
                    sizeof(struct aiVector3D), // attribute stride
                    (void *)0                  // attribute offset
                );
            }
        }

        for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; i++)
        {
            if (aiMesh->mColors[i] != NULL)
            {
                glGenBuffers(1, &(mesh->mColorBuffers[i]));
                glBindBuffer(GL_ARRAY_BUFFER, mesh->mColorBuffers[i]);
                glBufferData(GL_ARRAY_BUFFER, aiMesh->mNumVertices * sizeof(struct aiColor4D), aiMesh->mColors[i], GL_STATIC_DRAW);

                // TODO MULTIPLE COLORS
                long int index = -1l;
                CHECK(hashmap_find((void **)&index, &(shader->mAttributeMap), (unsigned char *)"aColor0", 7), return retval);

                if (index > -1l)
                {
                    glEnableVertexAttribArray(index);
                    glVertexAttribPointer(
                        index,                    // Attribute Index
                        4,                        // attribute size
                        GL_FLOAT,                 // attribute type
                        GL_FALSE,                 // should be normalized
                        sizeof(struct aiColor4D), // attribute stride
                        (void *)0                 // attribute offset
                    );
                }
            }
        }

        for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
        {
            if (aiMesh->mTextureCoords[i] != NULL)
            {

                // for (unsigned int j = 0; j < aiMesh->mNumVertices; j++)
                // {
                //     fprintf(stderr, "%u: %f,%f,%f\n", j, aiMesh->mTextureCoords[i][j].x, aiMesh->mTextureCoords[i][j].y, aiMesh->mTextureCoords[i][j].z);
                // }
                glGenBuffers(1, &(mesh->mTexCoordBuffers[i]));
                glBindBuffer(GL_ARRAY_BUFFER, mesh->mTexCoordBuffers[i]);
                glBufferData(GL_ARRAY_BUFFER, aiMesh->mNumVertices * sizeof(struct aiVector3D), aiMesh->mTextureCoords[i], GL_STATIC_DRAW);

                // TODO MULTIPLE TEXT COORDS
                long int index = -1l;
                CHECK(hashmap_find((void **)&index, &(shader->mAttributeMap), (unsigned char *)"aTexCoord0", 10), return retval);

                if (index > -1l)
                {
                    glEnableVertexAttribArray(index);
                    glVertexAttribPointer(
                        index,                     // Attribute Index
                        2,                         // attribute size
                        GL_FLOAT,                  // attribute type
                        GL_FALSE,                  // should be normalized
                        sizeof(struct aiVector3D), // attribute stride
                        (void *)0                  // attribute offset
                    );
                }
            }
        }
    }

    glBindVertexArray(GL_FALSE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_FALSE);
    glBindBuffer(GL_ARRAY_BUFFER, GL_FALSE);

    return 0;
}

int mesh_free(mesh_t *mesh)
{

    glDeleteBuffers(1, &(mesh->mElementBuffer));
    glDeleteBuffers(1, &(mesh->mVertexBuffer));
    glDeleteBuffers(1, &(mesh->mNormalBuffer));
    glDeleteBuffers(1, &(mesh->mTangentBuffer));
    glDeleteBuffers(1, &(mesh->mBitTangentBuffer));

    for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; i++)
    {
        glDeleteBuffers(1, &(mesh->mColorBuffers[i]));
    }

    for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
    {
        glDeleteBuffers(1, &(mesh->mTexCoordBuffers[i]));
    }

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
    model->mModelPath = model_name;
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
        CHECK(mesh_alloc(&(model->mMeshList[i]), shader, scene->mMeshes[i]), return retval);
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

    model->mModelPath = NULL;
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

const char *materialNames[] = {
    "None",
    "Diffuse",
    "Specular",
    "Ambient",
    "Emissive",
    "Height",
    "Normals",
    "Shininess",
    "Opacity",
    "Displacemet",
    "Lightmap",
    "Reflection",
    "BaseColor",
    "NormalCamera",
    "EmissiveColor",
    "Metalness",
    "DiffuseRoughness",
    "AmbientOcclusion",
    "Sheen",
    "ClearCoat",
    "Transmission"};

int material_draw(material_t *material, model_t *model)
{

    for (int type = aiTextureType_NONE; type < AI_TEXTURE_TYPE_MAX + 1; type++)
    {
        for (unsigned int index = 0; index < material->mTextureCount[type]; index++)
        {
            char name_buffer[1024];
            memset(name_buffer, 0, 1024);

            int unit = -1;
            CHECK(texture_activate(material->mTextureIndex[type][index], &unit), return retval);
            
            // int unit = 0;
            snprintf(name_buffer, 1023, "Material.%s[%d]", materialNames[type], index);
            CHECK(shader_set(model->mShader, name_buffer, I1, 1, &unit), return retval);
        }
    }

    for (unsigned int j = 0; j < material->mNumMeshes; j++)
    {
        CHECK(mesh_draw(&(model->mMeshList[material->mMeshes[j]]), model->mShader), return retval);
    }
    return 0;
}

int model_draw(model_t *model)
{

    mat4 transform = GLM_MAT4_IDENTITY_INIT;
    model_node_update_mesh_transforms(&(model->mRootNode), model, transform);

    CHECK(shader_use(model->mShader), return retval);
    for (unsigned int i = 0; i < model->mNumMaterials; i++)
    {
        CHECK(material_draw(&(model->mMaterialList[i]), model), return retval);
    }

    return 0;
}
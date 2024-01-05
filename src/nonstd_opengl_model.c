#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#define IMPORT_FLAGS                         \
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
        aiProcess_FindInvalidData |          \
        aiProcess_GenUVCoords |              \
        aiProcess_TransformUVCoords |        \
        aiProcess_OptimizeMeshes |           \
        aiProcess_OptimizeGraph |            \
        aiProcess_GenBoundingBoxes
#define AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE 80.0
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "nonstd.h"
#include "nonstd_opengl_shader.h"
#include "nonstd_opengl_model.h"

int process_node(model_node_t *model_node, const struct aiNode *node)
{
    memcpy(&(model_node->transformation), &(node->mTransformation), 16 * sizeof(float));

    model_node->numChildren = node->mNumChildren;
    safe_alloc((void **)&(model_node->children), model_node->numChildren * sizeof(model_node_t));

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(&(model_node->children[i]), node->mChildren[i]);
    }
    return 0;
}

int process_mesh(model_mesh_t *model_mesh, const struct aiMesh *mesh)
{

    if (!(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE))
    {
        return 1;
    }

    glGenVertexArrays(1, &(model_mesh->VAO));
    glGenBuffers(1, &(model_mesh->VBO));
    glGenBuffers(1, &(model_mesh->EBO));

    glBindVertexArray(model_mesh->VAO);

    if (mesh->mNumFaces > 0)
    {
        model_mesh->index_count = mesh->mNumFaces * 3;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_mesh->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model_mesh->index_count * sizeof(unsigned int), NULL, GL_STATIC_DRAW);

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            glBufferSubData(GL_ARRAY_BUFFER, i * 3, 3, mesh->mFaces[i].mIndices);
        }
    }

    if (mesh->mNumVertices > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, model_mesh->VBO);
        unsigned long int size;
        // TODO set vertex object size
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

        unsigned long int offset = 0;
        glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->mNumVertices * sizeof(struct aiVector3D), mesh->mVertices);
        glVertexAttribPointer(
            0,                         // Attrib Index
            3,                         // Attrib Size
            GL_FLOAT,                  // Atrib Type
            GL_FALSE,                  // Should be Normalized
            sizeof(struct aiVector3D), // Attrib Stride
            (void *)offset             // Attrib offset
        );
        offset += mesh->mNumVertices * sizeof(struct aiVector3D);

        if (mesh->mNormals)
        {
            glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->mNumVertices * sizeof(struct aiVector3D), mesh->mNormals);
            glVertexAttribPointer(
                1,                         // Attrib Index
                3,                         // Attrib Size
                GL_FLOAT,                  // Atrib Type
                GL_FALSE,                  // Should be Normalized
                sizeof(struct aiVector3D), // Attrib Stride
                (void *)offset             // Attrib offset
            );
            offset += mesh->mNumVertices * sizeof(struct aiVector3D);
        }

        if (mesh->mTangents)
        {
            glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->mNumVertices * sizeof(struct aiVector3D), mesh->mTangents);
            glVertexAttribPointer(
                2,                         // Attrib Index
                3,                         // Attrib Size
                GL_FLOAT,                  // Atrib Type
                GL_FALSE,                  // Should be Normalized
                sizeof(struct aiVector3D), // Attrib Stride
                (void *)offset             // Attrib offset
            );
            offset += mesh->mNumVertices * sizeof(struct aiVector3D);
        }

        if (mesh->mBitangents)
        {
            glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->mNumVertices * sizeof(struct aiVector3D), mesh->mBitangents);
            glVertexAttribPointer(
                3,                         // Attrib Index
                3,                         // Attrib Size
                GL_FLOAT,                  // Atrib Type
                GL_FALSE,                  // Should be Normalized
                sizeof(struct aiVector3D), // Attrib Stride
                (void *)offset             // Attrib offset
            );
            offset += mesh->mNumVertices * sizeof(struct aiVector3D);
        }

        // if (mesh->mColors != NULL)
        // {
        //     // AI_MAX_NUMBER_OF_COLOR_SETS
        //     glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->mNumVertices * AI_MAX_NUMBER_OF_COLOR_SETS * sizeof(struct aiColor4D), mesh->mColors);
        //     glVertexAttribPointer(
        //         4,                                                      // Attrib Index
        //         4,                                                      // Attrib Size
        //         GL_FLOAT,                                               // Atrib Type
        //         GL_FALSE,                                               // Should be Normalized
        //         AI_MAX_NUMBER_OF_COLOR_SETS * sizeof(struct aiColor4D), // Attrib Stride
        //         (void *)offset                                          // Attrib offset
        //     );
        //     offset += mesh->mNumVertices * AI_MAX_NUMBER_OF_COLOR_SETS * sizeof(struct aiColor4D);
        // }

        // if (mesh->mTextureCoords)
        // {
        //     // AI_MAX_NUMBER_OF_TEXTURECOORDS
        //     glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->mNumVertices * AI_MAX_NUMBER_OF_TEXTURECOORDS * sizeof(struct aiColor4D), mesh->mTextureCoords);
        //     glVertexAttribPointer(
        //         5,                                                         // Attrib Index
        //         4,                                                         // Attrib Size
        //         GL_FLOAT,                                                  // Atrib Type
        //         GL_FALSE,                                                  // Should be Normalized
        //         AI_MAX_NUMBER_OF_TEXTURECOORDS * sizeof(struct aiColor4D), // Attrib Stride
        //         (void *)offset                                             // Attrib offset
        //     );
        //     offset += mesh->mNumVertices * AI_MAX_NUMBER_OF_TEXTURECOORDS * sizeof(struct aiColor4D);
        // }

        // if (mesh->mTextureCoords)
        // {
        //     // AI_MAX_NUMBER_OF_TEXTURECOORDS
        //     glBufferSubData(GL_ARRAY_BUFFER, offset, mesh->mNumVertices * AI_MAX_NUMBER_OF_TEXTURECOORDS * sizeof(struct aiColor4D), mesh->mTextureCoords);
        //     glVertexAttribPointer(
        //         6,                                                         // Attrib Index
        //         4,                                                         // Attrib Size
        //         GL_FLOAT,                                                  // Atrib Type
        //         GL_FALSE,                                                  // Should be Normalized
        //         AI_MAX_NUMBER_OF_TEXTURECOORDS * sizeof(struct aiColor4D), // Attrib Stride
        //         (void *)offset                                             // Attrib offset
        //     );
        //     offset += mesh->mNumVertices * AI_MAX_NUMBER_OF_TEXTURECOORDS * sizeof(struct aiColor4D);
        // }
    }

    // mesh->mNumBones;
    // mesh->mBones;
    model_mesh->material_index = mesh->mMaterialIndex;
    return 0;
}

int process_material(model_material_t *model_material, const struct aiMaterial *material)
{

    for (unsigned int texture_type = 1; texture_type < AI_TEXTURE_TYPE_MAX; texture_type++)
    {

        float specStrength = 1.f; // default value, remains unmodified if we fail.
        aiGetMaterialFloat(material, AI_MATKEY_SHININESS_STRENGTH, (float *)&specStrength);

        unsigned int max_count = aiGetMaterialTextureCount(material, texture_type);
        model_material->count[texture_type] = max_count;
        safe_alloc((void **)&(model_material->texture_index[texture_type]), max_count * sizeof(unsigned long int));

        struct aiString path;
        for (unsigned int index = 0; index < max_count; index++)
        {
            aiGetMaterialTexture(material, texture_type, index, &path, NULL, NULL, NULL, NULL, NULL, NULL);
            get_load_texture(&(model_material->texture_index[texture_type][index]), path.data, path.length);
        }
    }

    return 0;
}

int nonstd_model_import(model_t *the_model, const char *pFile)
{

    const struct aiScene *scene = aiImportFile(pFile, IMPORT_FLAGS);

    // If the import failed, report it
    if (NULL == scene)
    {
        return 1;
    }

    the_model->mesh_count = scene->mNumMeshes;
    safe_alloc((void **)&(the_model->meshes), the_model->mesh_count * sizeof(model_mesh_t));
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        process_mesh(&(the_model->meshes[i]), scene->mMeshes[i]);
    }

    the_model->material_count = scene->mNumMaterials;
    safe_alloc((void **)&(the_model->materials), the_model->material_count * sizeof(model_material_t));
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        process_material(&(the_model->materials[i]), scene->mMaterials[i]);
    }

    // for(int i = 0; i < scene->mNumAnimations; i++)
    // {
    //     process_animations(scene, scene->mAnimations[i]);
    // }

    // for(int i = 0; i < scene->mNumTextures; i++)
    // {
    //     process_textures(scene, scene->mTextures[i]);
    // }

    // for(int i = 0; i < scene->mNumLights; i++)
    // {
    //     process_lights(scene, scene->mLights[i]);
    // }

    // for(int i = 0; i < scene->mNumCameras; i++)
    // {
    //     process_cameras(scene, scene->mCameras[i]);
    // }

    // process_node(scene->mRootNode);

    // We're done. Release all resources associated with this import
    aiReleaseImport(scene);
    return 0;
}

const char *texturenameformats[] = {
    "None",
    "Diffuse",
    "Specular",
    "Ambient",
    "Emmissive",
    "Height",
    "Normals",
    "Shininess",
    "Opacity",
    "Displacemet",
    "Lightmap",
    "Reflection",
    "BaseColor",
    "NormalCamera",
    "EmmisiveColor",
    "Metalness",
    "DiffuseRoughness",
    "AmbientOcclusion",
    "Sheen",
    "ClearCoat",
    "Transmission"};

int nondst_activate_material(const shader_t *shader, const model_material_t *material)
{
    char nameBuff[1024];
    for (unsigned int texture_type = 1; texture_type < AI_TEXTURE_TYPE_MAX; texture_type++)
    {
        for (unsigned int index = 0; index < material->count[texture_type]; index++)
        {
            memset(nameBuff, 0, sizeof(nameBuff));

            snprintf(nameBuff, 1023, "Material.%s[%d]", texturenameformats[texture_type], index);
            int unit;
            texture_activate(material->texture_index[texture_type][index], &unit);
            shader_set(shader, nameBuff, I1, 1, &unit);
        }
    }
    return 0;
}

int nonstd_mesh_draw(const shader_t *shader, const model_t *model, const model_mesh_t *mesh)
{
    nondst_activate_material(shader, &(model->materials[mesh->material_index]));
    // draw mesh
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    return 0;
}

int nonstd_model_draw(const shader_t *shader, const model_t *model)
{
    for (unsigned int i = 0; i < model->mesh_count; i++)
    {
        nonstd_mesh_draw(shader, model, &(model->meshes[i]));
    }
    return 0;
}
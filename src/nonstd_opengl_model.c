#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#define AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE 80.0
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "nonstd_opengl_model.h"

int process_node(const struct aiNode *node)
{

    // node->mTransformation;
    // node->mParent;

    // for (unsigned int i = 0; i < node->mNumMeshes; i++)
    // {
    //     node->mMeshes[i];
    // }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i]);
    }
    return 0;
}

int process_mesh(const struct aiMesh *mesh)
{

    if (!(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE))
    {
        return 1;
    }

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    if (mesh->mNumFaces > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->mNumFaces * 3 * sizeof(unsigned int), NULL, GL_STATIC_DRAW);

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            glBufferSubData(GL_ARRAY_BUFFER, i * 3, 3, mesh->mFaces[i].mIndices);
        }
    }

    if (mesh->mNumVertices > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

    // mesh->mMaterialIndex;
    return 0;
}

int process_material(const struct aiMaterial *material)
{
    for (unsigned int i = 0; i < material->mNumProperties; i++)
    {
        unsigned int index = material->mProperties[i]->mIndex;
        index = index;
    }
    return 0;
}

int DoTheImportThing(const char *pFile)
{

    unsigned int importFlags = aiProcess_CalcTangentSpace |
                               aiProcess_JoinIdenticalVertices |
                               aiProcess_Triangulate |
                               aiProcess_GenSmoothNormals |
                               aiProcess_SplitLargeMeshes |
                               aiProcess_LimitBoneWeights |
                               aiProcess_ValidateDataStructure |
                               aiProcess_ImproveCacheLocality |
                               aiProcess_RemoveRedundantMaterials |
                               aiProcess_SortByPType |
                               aiProcess_FindInvalidData |
                               aiProcess_GenUVCoords |
                               aiProcess_TransformUVCoords |
                               aiProcess_OptimizeMeshes |
                               aiProcess_OptimizeGraph |
                               aiProcess_GenBoundingBoxes;

    const struct aiScene *scene = aiImportFile(pFile, importFlags);

    // If the import failed, report it
    if (NULL == scene)
    {
        return 1;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        process_mesh(scene->mMeshes[i]);
    }

    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        process_material(scene->mMaterials[i]);
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

    // Now we can access the file's contents
    // DoTheSceneProcessing(scene);

    process_node(scene->mRootNode);

    // We're done. Release all resources associated with this import
    aiReleaseImport(scene);
    return 0;
}
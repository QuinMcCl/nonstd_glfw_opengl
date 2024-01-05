#ifndef NONSTD_OPENGL_MODEL_H
#define NONSTD_OPENGL_MODEL_H

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <cglm/cglm.h> /* for inline */
#include "nonstd_opengl_texture.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct model_node_s
    {
        mat4 transformation;
        unsigned int numChildren;
        struct model_node_s *children;

    } model_node_t;

    typedef struct model_material_s
    {
        unsigned int count[AI_TEXTURE_TYPE_MAX];
        unsigned long int *texture_index[AI_TEXTURE_TYPE_MAX];
    } model_material_t;

    typedef struct model_mesh_s
    {
        unsigned int VAO, VBO, EBO;
        unsigned int index_count;
        unsigned int material_index;
    } model_mesh_t;

    typedef struct model_s
    {
        // model_node_t * root_node;
        unsigned int mesh_count;
        model_mesh_t *meshes;
        unsigned int material_count;
        model_material_t *materials;
    } model_t;

    int process_mesh(model_mesh_t *model_mesh, const struct aiMesh *mesh);
    int process_material(model_material_t *model_material, const struct aiMaterial *material);
    int process_node(model_node_t *model_node, const struct aiNode *node);
    int nonstd_model_import(model_t *the_model, const char *pFile);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
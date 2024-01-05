#ifndef NONSTD_OPENGL_MODEL_H
#define NONSTD_OPENGL_MODEL_H

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#ifdef __cplusplus
extern "C"
{
#endif

    int process_mesh(const struct aiMesh *mesh);
    int process_material(const struct aiMaterial *material);
    int process_node(const struct aiNode *node);
    int DoTheImportThing(const char *pFile);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
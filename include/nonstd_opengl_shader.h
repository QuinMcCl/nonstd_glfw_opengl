#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

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

typedef struct nonstd_opengl_shader_s
{
    unsigned int ID;
} shader_t;

// constructor generates the shader on the fly
// ------------------------------------------------------------------------
void shader_init(shader_t *shader, const char *vertexPath, const char *fragmentPath);
void shader_cleanup(shader_t *shader);
// activate the shader
// ------------------------------------------------------------------------
void shader_use(const shader_t *shader);
void shader_bindBuffer(const shader_t *shader, const char *name, const unsigned int index);

void shader_set(const shader_t *shader, const char *name, const shader_set_type_t type, const int count, void *value);

#endif
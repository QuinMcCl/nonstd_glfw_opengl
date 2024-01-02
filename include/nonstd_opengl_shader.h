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
} nonstd_opengl_shader_t;

// constructor generates the shader on the fly
// ------------------------------------------------------------------------
void nonstd_opengl_shader_init(nonstd_opengl_shader_t *shader, const char *vertexPath, const char *fragmentPath);
void nonstd_opengl_shader_cleanup(nonstd_opengl_shader_t *shader);
// activate the shader
// ------------------------------------------------------------------------
void nonstd_opengl_shader_use(nonstd_opengl_shader_t *shader);
void nonstd_opengl_shader_bindBuffer(nonstd_opengl_shader_t *shader, char *name, unsigned int index);

void nonstd_opengl_shader_set(nonstd_opengl_shader_t *shader, char *name, shader_set_type_t type, int count, void *value);

#endif
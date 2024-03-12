#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <GL/glew.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "nonstd.h"
#include "nonstd_opengl_shader.h"

#define ON_ERROR return errno;
int readFile(const char *filename, char **const buffer, long int *len)
{
    assert(filename != NULL);
    assert(buffer != NULL);
    assert(len != NULL);

    FILE *fp = fopen(filename, "rb");
    CHECK_ERR((fp == NULL) ? EINVAL : 0);

    fseek(fp, 0, SEEK_END);
    long int fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    CHECK_ERR(safe_alloc((void **)buffer, fileSize + 1));
    memset((void *)*buffer, '\0', fileSize + 1);
    rewind(fp);
    fread((void *)*buffer, fileSize, 1, fp);
    fclose(fp);
    *len = fileSize;
    return 0;
}

int CompileShader(unsigned int type, const char *const *shaderCode, unsigned int *shaderProgram)
{
    unsigned int id = GL_FALSE;
    GLint success = GL_FALSE;
    GLchar infoLog[1024] = "";

    id = glCreateShader(type);
    glShaderSource(id, 1, shaderCode, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(id, 1024, NULL, infoLog);
        fprintf(stderr, "%s at %s:%d %s\n", infoLog, __FILE__, __LINE__, "glCompileShader()");
        return GL_FALSE;
    }
    *shaderProgram = id;
    return 0;
}

unsigned int linkShader(unsigned int vertex, unsigned int fragment)
{
    unsigned int id = GL_FALSE;
    GLint success = GL_FALSE;
    GLchar infoLog[1024] = "";

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(id, 1024, NULL, infoLog);
        fprintf(stderr, "%s at %s:%d %s\n", infoLog, __FILE__, __LINE__, "glLinkProgram()");
        return GL_FALSE;
    }
    return id;
}

int shader_init(shader_t *shader, const char *vertexPath, const char *fragmentPath)
{
    shader->ID = GL_FALSE;
    // 1. retrieve the vertex/fragment source code from filePath
    char *vertexCode = NULL;
    char *fragmentCode = NULL;

    long int vertex_size = 0l;
    long int fragmt_size = 0l;

    CHECK_ERR(readFile(vertexPath, (char **const)&vertexCode, &vertex_size));
    CHECK_ERR(readFile(fragmentPath, (char **const)&fragmentCode, &fragmt_size));

    // 2. compile shaders
    unsigned int vertex, fragment;
    CHECK_ERR(CompileShader(GL_VERTEX_SHADER, (const char *const *)&vertexCode, &vertex));
    CHECK_ERR(CompileShader(GL_FRAGMENT_SHADER, (const char *const *)&fragmentCode, &fragment));

    CHECK_ERR(safe_free((void **)&vertexCode));
    CHECK_ERR(safe_free((void **)&fragmentCode));

    // 3. link shaders
    if (vertex != GL_FALSE && fragment != GL_FALSE)
        shader->ID = linkShader(vertex, fragment);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return 0;
}

int shader_free(shader_t *shader)
{
    assert(shader != NULL);
    if (shader->ID != GL_FALSE)
    {
        glDeleteShader(shader->ID);
        shader->ID = GL_FALSE;
    }
    return 0;
}

int shader_use(const shader_t *shader)
{
    assert(shader != NULL);
    glUseProgram(shader->ID);
    return 0;
}

int shader_bindBuffer(const shader_t *shader, const char *name, const unsigned int index)
{
    assert(shader != NULL);
    glUniformBlockBinding(shader->ID, glGetUniformBlockIndex(shader->ID, name), index);
    return 0;
}

int shader_set(const shader_t *shader, const char *name, const shader_set_type_t type, const int count, void *value)
{
    assert(shader != NULL);
    assert(name != NULL);
    assert(count != 0);
    assert(value != NULL);

    GLint loc = glGetUniformLocation(shader->ID, name);
    switch (type)
    {
    case F1:
        glUniform1fv(loc, count, (const GLfloat *)value);
        break;
    case F2:
        glUniform2fv(loc, count, (const GLfloat *)value);
        break;
    case F3:
        glUniform3fv(loc, count, (const GLfloat *)value);
        break;
    case F4:
        glUniform4fv(loc, count, (const GLfloat *)value);
        break;
    case I1:
        glUniform1iv(loc, count, (const GLint *)value);
        break;
    case I2:
        glUniform2iv(loc, count, (const GLint *)value);
        break;
    case I3:
        glUniform3iv(loc, count, (const GLint *)value);
        break;
    case I4:
        glUniform4iv(loc, count, (const GLint *)value);
        break;
    case UI1:
        glUniform1uiv(loc, count, (const GLuint *)value);
        break;
    case UI2:
        glUniform2uiv(loc, count, (const GLuint *)value);
        break;
    case UI3:
        glUniform3uiv(loc, count, (const GLuint *)value);
        break;
    case UI4:
        glUniform4uiv(loc, count, (const GLuint *)value);
        break;
    case M2:
        glUniformMatrix2fv(loc, count, GL_FALSE, (const GLfloat *)value);
        break;
    case M3:
        glUniformMatrix3fv(loc, count, GL_FALSE, (const GLfloat *)value);
        break;
    case M4:
        glUniformMatrix4fv(loc, count, GL_FALSE, (const GLfloat *)value);
        break;

    default:
        fprintf(stderr, "UNKNOWN TYPE at %s:%d %s\n", __FILE__, __LINE__, "shader_set()");
        return GL_FALSE;
    }
    return 0;
}

#undef ON_ERROR
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include "nonstd.h"
#include "nonstd_opengl_shader.h"

long int readFile(const char *filename, char **const buffer)
{
    FILE *fp = fopen(filename, "rb");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        long fileSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        safe_alloc((void **)buffer, fileSize + 1);
        memset((void *)*buffer, '\0', fileSize + 1);
        rewind(fp);
        fread((void *)*buffer, fileSize, 1, fp);
        fclose(fp);
        return fileSize;
    }
    return -1;
}

unsigned int CompileShader(unsigned int type, const char *const *shaderCode)
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
        fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR::%s\n", infoLog);
        return GL_FALSE;
    }
    return id;
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
        fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR::LINKING::%s\n", infoLog);
        return GL_FALSE;
    }
    return id;
}

void nonstd_opengl_shader_init(nonstd_opengl_shader_t *shader, const char *vertexPath, const char *fragmentPath)
{
    shader->ID = GL_FALSE;
    // 1. retrieve the vertex/fragment source code from filePath
    char *vertexCode = NULL;
    char *fragmentCode = NULL;

    long int vertex_size = readFile(vertexPath, (char **const)&vertexCode);
    long int fragmt_size = readFile(fragmentPath, (char **const)&fragmentCode);

    // 2. compile shaders
    unsigned int vertex, fragment;
    vertex = CompileShader(GL_VERTEX_SHADER, (const char *const *)&vertexCode);
    fragment = CompileShader(GL_FRAGMENT_SHADER, (const char *const *)&fragmentCode);
    safe_free((void **)&vertexCode, vertex_size);
    safe_free((void **)&fragmentCode, fragmt_size);

    // 3. link shaders
    if (vertex != GL_FALSE && fragment != GL_FALSE)
        shader->ID = linkShader(vertex, fragment);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void nonstd_opengl_shader_cleanup(nonstd_opengl_shader_t *shader)
{
    if (shader->ID != GL_FALSE)
    {
        glDeleteShader(shader->ID);
        shader->ID = GL_FALSE;
    }
}

void nonstd_opengl_shader_use(nonstd_opengl_shader_t *shader)
{
    glUseProgram(shader->ID);
}
void nonstd_opengl_shader_bindBuffer(nonstd_opengl_shader_t *shader, char *name, unsigned int index)
{

    glUniformBlockBinding(shader->ID, glGetUniformBlockIndex(shader->ID, name), index);
}

void nonstd_opengl_shader_set(nonstd_opengl_shader_t *shader, char *name, shader_set_type_t type, int count, void *value)
{
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
        break;
    }
}
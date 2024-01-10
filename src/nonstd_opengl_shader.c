#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <GL/glew.h>

#include "nonstd.h"
#include "nonstd_opengl_shader.h"

int readFile(const char *filename, char **const buffer, long int *len)
{
    FILE *fp = fopen(filename, "rb");
    if (fp != NULL)

        THROW_ERR((fp == NULL), "COULD NOT OPEN FILE", return retval);

    fseek(fp, 0, SEEK_END);
    long int fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    CHECK(safe_alloc((void **)buffer, fileSize + 1), return retval);
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
        THROW_ERR((!success), infoLog, return retval);
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
        fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR::LINKING::%s\n", infoLog);
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

    CHECK(readFile(vertexPath, (char **const)&vertexCode, &vertex_size), return retval);
    CHECK(readFile(fragmentPath, (char **const)&fragmentCode, &fragmt_size), return retval);

    // 2. compile shaders
    unsigned int vertex, fragment;
    CHECK(CompileShader(GL_VERTEX_SHADER, (const char *const *)&vertexCode, &vertex), return retval);
    CHECK(CompileShader(GL_FRAGMENT_SHADER, (const char *const *)&fragmentCode, &fragment), return retval);

    CHECK(safe_free((void **)&vertexCode, vertex_size), return retval);
    CHECK(safe_free((void **)&fragmentCode, fragmt_size), return retval);

    // 3. link shaders
    if (vertex != GL_FALSE && fragment != GL_FALSE)
        shader->ID = linkShader(vertex, fragment);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    unsigned int numAttributes = 0;
    int lenAttributeMax = 0;
    unsigned char *attributeNameBuffer = NULL;

    glGetProgramiv(shader->ID, GL_ACTIVE_ATTRIBUTES, (GLint *)&numAttributes);
    glGetProgramiv(shader->ID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &lenAttributeMax);

    shader->mNumAttributes = numAttributes;
    lenAttributeMax++;

    CHECK(hashmap_alloc(&(shader->mAttributeMap), NULL, numAttributes, numAttributes), return retval);
    CHECK(safe_alloc((void **)&attributeNameBuffer, lenAttributeMax), return retval);
    CHECK(safe_alloc((void **)&(shader->mAttributeList), shader->mNumAttributes * sizeof(shader_attrubute_t)), return retval);

    for (unsigned long int i = 0; i < numAttributes; i++)
    {
        int len = 0;
        glGetActiveAttrib(shader->ID, i, lenAttributeMax, &len, &(shader->mAttributeList[i].mAttributeSize), &(shader->mAttributeList[i].mAttributeType), (GLchar *)attributeNameBuffer);
        long int location = glGetAttribLocation(shader->ID, (const char *)attributeNameBuffer);
        CHECK(hashmap_add((void *)location, &(shader->mAttributeMap), attributeNameBuffer, len), return retval);
    }

    CHECK(safe_free((void **)&attributeNameBuffer, lenAttributeMax), return retval);

    return 0;
}

int shader_free(shader_t *shader)
{
    if (shader->ID != GL_FALSE)
    {
        glDeleteShader(shader->ID);
        shader->ID = GL_FALSE;
    }
    CHECK(hashmap_free(&(shader->mAttributeMap)), return retval);
    CHECK(safe_free((void **)&(shader->mAttributeList), shader->mNumAttributes * sizeof(shader_attrubute_t)), return retval);

    return 0;
}

int shader_use(const shader_t *shader)
{
    THROW_ERR((shader == NULL), "NULL SHADER PTR", return retval);
    glUseProgram(shader->ID);
    return 0;
}

int shader_bindBuffer(const shader_t *shader, const char *name, const unsigned int index)
{
    THROW_ERR((shader == NULL), "NULL SHADER PTR", return retval);
    glUniformBlockBinding(shader->ID, glGetUniformBlockIndex(shader->ID, name), index);
    return 0;
}

int shader_set(const shader_t *shader, const char *name, const shader_set_type_t type, const int count, void *value)
{
    THROW_ERR((shader == NULL), "NULL SHADER PTR", return retval);
    THROW_ERR((name == NULL), "NULL NAME PTR", return retval);
    THROW_ERR((count == 0), "INVALID COUNT", return retval);
    THROW_ERR((value == NULL), "NULL VALUE PTR", return retval);

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
        THROW_ERR((name == NULL), "UNKNOWN TYPE", return retval);
        break;
    }
    return 0;
}
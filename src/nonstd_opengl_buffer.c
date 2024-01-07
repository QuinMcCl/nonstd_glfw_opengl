#include <GL/glew.h>
#include <GL/glut.h>

#include "nonstd.h"
#include "nonstd_opengl_buffer.h"

freelist_t binding_points;
int bindingpoints_initialized = GL_FALSE;

int nonstd_opengl_ubo_bindingpoints_alloc()
{
    if (bindingpoints_initialized == GL_TRUE)
        return 0;
    long int max_binding_points = 0;
    glGetInteger64v(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_binding_points);

#ifdef ERROR_CHECKING
    THROW_ERR((max_binding_points <= 0), "INVALID MAX BINDING POINTS", return retval);
#endif

    CHECK(freelist_alloc(&binding_points, max_binding_points, sizeof(unsigned long int)), return retval);
    bindingpoints_initialized = GL_TRUE;
    return 0;
}

int nonstd_opengl_ubo_bindingpoints_free()
{
    if (bindingpoints_initialized != GL_TRUE)
        return 0;
    CHECK(freelist_free(&binding_points), return retval);
    bindingpoints_initialized = GL_FALSE;
    return 0;
}

int nonstd_opengl_ubo_init(nonstd_opengl_ubo_t *ubo, const char *name, const unsigned int maxSize, unsigned int usage)
{
#ifdef ERROR_CHECKING
    THROW_ERR((bindingpoints_initialized != GL_TRUE), "INITIALIZE BINDING POINTS FIRST", return retval);
    THROW_ERR((ubo == NULL), "NULL UBO PTR", return retval);
    THROW_ERR((name == NULL), "NULL name PTR", return retval);
    THROW_ERR((maxSize == 0), "ZERO maxSize", return retval);
#endif
    unsigned long int next_free = -1;
    CHECK(freelist_aquire(&(next_free), &binding_points), return retval);
    ubo->bindingPoint = next_free;
    ubo->uboBlock = GL_FALSE;
    ubo->maxSize = maxSize;
    ubo->usage = usage;
    ubo->name = name;
    // allocate buffer
    glGenBuffers(1, &(ubo->uboBlock));
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->uboBlock);
    glBufferData(GL_UNIFORM_BUFFER, ubo->maxSize, NULL, ubo->usage);

    // bind the buffer
    glBindBufferBase(GL_UNIFORM_BUFFER, ubo->bindingPoint, ubo->uboBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return 0;
}

int nonstd_opengl_ubo_cleanup(nonstd_opengl_ubo_t *ubo)
{
#ifdef ERROR_CHECKING
    THROW_ERR((bindingpoints_initialized != GL_TRUE), "INITIALIZE BINDING POINTS FIRST", return retval);
    THROW_ERR((ubo == NULL), "NULL UBO PTR", return retval);
#endif
    unsigned long int last_free = ubo->bindingPoint;
    CHECK(freelist_release(&(last_free), &binding_points), return retval);
    glDeleteBuffers(1, &(ubo->uboBlock));
    ubo->uboBlock = GL_FALSE;
    ubo->maxSize = 0;
    ubo->usage = 0;
    ubo->name = NULL;
    return 0;
}

int nonstd_opengl_ubo_fill(nonstd_opengl_ubo_t *ubo, const void *items, unsigned int size, unsigned int offset)
{
#ifdef ERROR_CHECKING
    THROW_ERR((bindingpoints_initialized != GL_TRUE), "INITIALIZE BINDING POINTS FIRST", return retval);
    THROW_ERR((ubo == NULL), "NULL UBO PTR", return retval);
    THROW_ERR((size + offset > ubo->maxSize), "BUFFER OVERFLOW", return retval);
#endif
    // fill the buffer
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->uboBlock);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, items);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return 0;
}

int nonstd_opengl_ubo_getName(nonstd_opengl_ubo_t *ubo, const char **buffer)
{
#ifdef ERROR_CHECKING
    THROW_ERR((bindingpoints_initialized != GL_TRUE), "INITIALIZE BINDING POINTS FIRST", return retval);
    THROW_ERR((ubo == NULL), "NULL UBO PTR", return retval);
#endif
    *buffer = ubo->name;
    return 0;
}

int nonstd_opengl_ubo_getBlockBindingIndex(nonstd_opengl_ubo_t *ubo, unsigned int *index)
{
#ifdef ERROR_CHECKING
    THROW_ERR((bindingpoints_initialized != GL_TRUE), "INITIALIZE BINDING POINTS FIRST", return retval);
    THROW_ERR((ubo == NULL), "NULL UBO PTR", return retval);
#endif
    *index = ubo->bindingPoint;
    return 0;
}
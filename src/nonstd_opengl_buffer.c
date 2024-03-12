#include <GL/glew.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "nonstd.h"
#include "nonstd_opengl_buffer.h"

#define ON_ERROR return errno;
pthread_mutex_t lock_binding_point_array;
long int max_binding_points = 0;
long int index_binding_point = -1;
long int *binding_point_array = NULL;

int bindingpoints_initialized = GL_FALSE;
int nonstd_opengl_ubo_bindingpoints_alloc()
{
    if (bindingpoints_initialized == GL_TRUE)
        return 0;
    glGetInteger64v(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_binding_points);
    CHECK_ERR(max_binding_points <= 0
                  ? EPERM
                  : EXIT_SUCCESS);

    CHECK_ERR(safe_alloc((void **)&binding_point_array, max_binding_points * sizeof(long int)));

    for (long int index = 0; index < max_binding_points; index++)
    {
        binding_point_array[index] = index;
    }
    index_binding_point = 0;

    bindingpoints_initialized = GL_TRUE;
    return 0;
}

int nonstd_opengl_ubo_bindingpoints_free()
{
    if (bindingpoints_initialized != GL_TRUE)
        return 0;
    CHECK_ERR(safe_free((void **)&binding_point_array));
    index_binding_point = -1;
    max_binding_points = 0;
    bindingpoints_initialized = GL_FALSE;
    return 0;
}

int nonstd_opengl_ubo_init(nonstd_opengl_ubo_t *ubo, const char *name, const unsigned int maxSize, unsigned int usage)
{
    assert(bindingpoints_initialized == GL_TRUE && "BINDING POINTS UN INITIALIZED");
    assert(ubo != NULL && "NULL UBO POINTER");
    assert(name != NULL && "NULL NAME POINTER");
    assert(maxSize > 0 && "ZERO SIZE");

    long int next_free = -1;
    int retval = 0;
    CHECK_ERR(pthread_mutex_lock(&(lock_binding_point_array)));
    if (index_binding_point >= 0 && index_binding_point < max_binding_points)
    {
        next_free = binding_point_array[index_binding_point++];
    }
    else
    {
        retval = EINVAL;
    }
    CHECK_ERR(pthread_mutex_unlock(&(lock_binding_point_array)));
    CHECK_ERR(retval);
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
    assert(bindingpoints_initialized == GL_TRUE && "BINDING POINTS UN INITIALIZED");
    assert(ubo != NULL && "NULL UBO POINTER");

    int retval = 0;
    CHECK_ERR(pthread_mutex_lock(&(lock_binding_point_array)));
    if (ubo->bindingPoint >= 0 && ubo->bindingPoint < max_binding_points)
    {
        binding_point_array[--index_binding_point] = ubo->bindingPoint;
    }
    else
    {
        retval = EINVAL;
    }
    CHECK_ERR(pthread_mutex_unlock(&(lock_binding_point_array)));
    CHECK_ERR(retval);

    glDeleteBuffers(1, &(ubo->uboBlock));
    ubo->uboBlock = GL_FALSE;
    ubo->maxSize = 0;
    ubo->usage = 0;
    ubo->name = NULL;
    return 0;
}

int nonstd_opengl_ubo_fill(nonstd_opengl_ubo_t *ubo, const void *items, unsigned int size, unsigned int offset)
{
    assert(bindingpoints_initialized == GL_TRUE && "BINDING POINTS UN INITIALIZED");
    assert(ubo != NULL && "NULL UBO POINTER");
    assert(size + offset <= ubo->maxSize && "INVALID SIZE");

    // fill the buffer
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->uboBlock);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, items);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return 0;
}

#undef ON_ERROR
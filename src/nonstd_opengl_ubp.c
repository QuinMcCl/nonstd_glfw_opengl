
#include "nonstd.h"
#include "nonstd_opengl_ubo.h"

freelist_t binding_points;
int bindingpoints_initialized = GL_FALSE;

int nonstd_opengl_ubo_bindingpoints_alloc()
{
    freelist_alloc(&binding_points, GL_MAX_UNIFORM_BUFFER_BINDINGS, sizeof(unsigned int));
    bindingpoints_initialized = GL_TRUE;
    return 0;
}

int nonstd_opengl_ubo_bindingpoints_free()
{
    freelist_free(&binding_points);
    bindingpoints_initialized = GL_FALSE;
    return 0;
}

int nonstd_opengl_ubo_init(nonstd_opengl_ubo_t *ubo, const char *name, const unsigned int maxSize, unsigned int usage)
{
    unsigned long int next_free = -1;
    freelist_aquire(&(next_free), &binding_points);
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

    unsigned long int last_free = ubo->bindingPoint;
    freelist_release(&(last_free), &binding_points);
    glDeleteBuffers(1, &(ubo->uboBlock));
    ubo->uboBlock = GL_FALSE;
    ubo->maxSize = 0;
    ubo->usage = 0;
    ubo->name = NULL;
    return 0;
}

int nonstd_opengl_ubo_fill(nonstd_opengl_ubo_t *ubo, const void *items, unsigned int size, unsigned int offset)
{
    if (size + offset > ubo->maxSize)
    {
        glDeleteBuffers(1, &(ubo->uboBlock));
        ubo->uboBlock = GL_FALSE;
        ubo->maxSize = size + offset;
        glGenBuffers(1, &(ubo->uboBlock));
        glBindBuffer(GL_UNIFORM_BUFFER, ubo->uboBlock);
        glBufferData(GL_UNIFORM_BUFFER, ubo->maxSize, NULL, ubo->usage);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, items);
        glBindBufferBase(GL_UNIFORM_BUFFER, ubo->bindingPoint, ubo->uboBlock);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    else
    {
        // fill the buffer
        glBindBuffer(GL_UNIFORM_BUFFER, ubo->uboBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, items);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    return 0;
}

int nonstd_opengl_ubo_getName(nonstd_opengl_ubo_t *ubo, const char **buffer)
{
    *buffer = ubo->name;
    return 0;
}

int nonstd_opengl_ubo_getBlockBindingIndex(nonstd_opengl_ubo_t *ubo, unsigned int *index)
{
    *index = ubo->bindingPoint;
    return 0;
}
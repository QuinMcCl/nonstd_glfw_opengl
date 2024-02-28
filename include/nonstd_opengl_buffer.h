#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct nonstd_opengl_ubo_s
    {

        int bindingPoint;
        unsigned int uboBlock;
        unsigned int maxSize;
        unsigned int usage;
        const char *name;
    } nonstd_opengl_ubo_t;

    int nonstd_opengl_ubo_bindingpoints_alloc();
    int nonstd_opengl_ubo_bindingpoints_free();

    int nonstd_opengl_ubo_init(nonstd_opengl_ubo_t *ubo, const char *name, const unsigned int maxSize, unsigned int usage);
    int nonstd_opengl_ubo_cleanup(nonstd_opengl_ubo_t *ubo);

    int nonstd_opengl_ubo_fill(nonstd_opengl_ubo_t *ubo, const void *items, unsigned int size, unsigned int offset);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
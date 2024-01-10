#ifndef NONSTD_GLFW_WINDOW_H
#define NONSTD_GLFW_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct window_s
    {
        GLFWwindow *window;
        int width;
        int height;
        float aspect;
        void *old_user_ptr;

    } window_t;

    int window_init(window_t *window, unsigned int width, unsigned int height, const char *title, float clear_red, float clear_green, float clear_blue, float clear_alpha);
    void window_cleanup(window_t *window);

    void window_swap(window_t *window);
    void window_clear(window_t *window);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
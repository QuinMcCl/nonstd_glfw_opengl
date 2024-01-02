#ifndef NONSTD_GLFW_WINDOW_H
#define NONSTD_GLFW_WINDOW_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include "nonstd_glfw.h"

typedef struct nonstd_glfw_window_s
{
    nonstd_glfw_t base;
    unsigned int width;
    unsigned int height;
    GLFWwindow *window;
    float clear_color[4];

} nonstd_glfw_window_t;

int nonstd_glfw_window_init(nonstd_glfw_window_t *window, unsigned int width, unsigned int height, const char *title, float clear_red, float clear_green, float clear_blue, float clear_alpha);

int nonstd_glfw_window_swap(nonstd_glfw_window_t *nonstd_glfw_window);
int nonstd_glfw_window_get_should_close(nonstd_glfw_window_t *window, int * should_close);
int nonstd_glfw_window_set_should_close(nonstd_glfw_window_t *window, int *should_close);

int nonstd_glfw_window_set_clear_color(nonstd_glfw_window_t *window, float red, float green, float blue, float alpha);
int nonstd_glfw_window_draw(void *ptr);
int nonstd_glfw_window_cleanup(void *ptr);
int nonstd_glfw_window_event_handler(void *ptr, void *e);

#endif
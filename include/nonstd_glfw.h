#ifndef NONSTD_GLFW_H
#define NONSTD_GLFW_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

typedef int (*nonstd_glfw_update_t)(void *nonstd_glfw, double dt);
typedef int (*nonstd_glfw_draw_t)(void *nonstd_glfw);
typedef int (*nonstd_glfw_cleanup_t)(void *nonstd_glfw);
typedef int (*nonstd_glfw_event_handler_t)(void *nonstd_glfw, void *event);

typedef struct nonstd_glfw_s
{
    nonstd_glfw_update_t update;
    nonstd_glfw_draw_t draw;
    nonstd_glfw_cleanup_t cleanup;
    nonstd_glfw_event_handler_t event_handler;
    struct nonstd_glfw_s *sibling;
    struct nonstd_glfw_s *child;
} nonstd_glfw_t;

int nonstd_glfw_init(nonstd_glfw_t *nonstd_glfw);

int nonstd_glfw_adopt(nonstd_glfw_t *parent, nonstd_glfw_t *child);
int nonstd_glfw_orphan(nonstd_glfw_t *parent, nonstd_glfw_t *child);

int nonstd_glfw_update(void *nonstd_glfw, double dt);
int nonstd_glfw_draw(void *nonstd_glfw);
int nonstd_glfw_cleanup(void *nonstd_glfw);
int nonstd_glfw_event_handler(void *nonstd_glfw, void *e);

#endif
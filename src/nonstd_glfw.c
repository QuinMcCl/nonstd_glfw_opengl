#include "nonstd_glfw.h"

int nonstd_glfw_init(nonstd_glfw_t *nonstd_glfw)
{
    nonstd_glfw->update = nonstd_glfw_update;
    nonstd_glfw->draw = nonstd_glfw_draw;
    nonstd_glfw->cleanup = nonstd_glfw_cleanup;
    nonstd_glfw->event_handler = nonstd_glfw_event_handler;
    nonstd_glfw->sibling = NULL;
    nonstd_glfw->child = NULL;

    return 0;
}

int nonstd_glfw_adopt(nonstd_glfw_t *parent, nonstd_glfw_t *child)
{
    if (parent == NULL)
    {
        return 1;
    }
    if (child == NULL)
    {
        return 1;
    }

    if (parent->child == NULL)
    {
        parent->child = child;
    }
    else
    {
        nonstd_glfw_t *last_child = parent->child;
        while (last_child->sibling != NULL)
        {
            last_child = last_child->sibling;
        }
        last_child->sibling = child;
    }
    return 0;
}
int nonstd_glfw_orphan(nonstd_glfw_t *parent, nonstd_glfw_t *child)
{
    if (parent == NULL)
    {
        return 1;
    }
    if (parent->child == NULL)
    {
        return 1;
    }
    if (child == NULL)
    {
        return 1;
    }

    if (parent->child == child)
    {
        parent->child = child->sibling;
        child->sibling = NULL;
    }
    else
    {
        nonstd_glfw_t *last_child = parent->child;
        while (last_child->sibling != NULL && last_child->sibling != child)
        {
            last_child = last_child->sibling;
        }
        if (last_child->sibling != child)
        {
            return 1;
        }

        last_child->sibling = child->sibling;
        child->sibling = NULL;
    }
    return 0;
}

int nonstd_glfw_update(void *ptr, double dt)
{
    nonstd_glfw_t *nonstd_glfw = (nonstd_glfw_t *)ptr;
    if (nonstd_glfw->child != NULL && nonstd_glfw->child->update != NULL)
        nonstd_glfw->child->update(nonstd_glfw->child, dt);
    if (nonstd_glfw->sibling != NULL && nonstd_glfw->sibling->update != NULL)
        nonstd_glfw->sibling->update(nonstd_glfw->sibling, dt);
    return 0;
}
int nonstd_glfw_draw(void *ptr)
{
    nonstd_glfw_t *nonstd_glfw = (nonstd_glfw_t *)ptr;
    if (nonstd_glfw->child != NULL && nonstd_glfw->child->draw != NULL)
        nonstd_glfw->child->draw(nonstd_glfw->child);
    if (nonstd_glfw->sibling != NULL && nonstd_glfw->sibling->draw != NULL)
        nonstd_glfw->sibling->draw(nonstd_glfw->sibling);
    return 0;
}
int nonstd_glfw_cleanup(void *ptr)
{
    nonstd_glfw_t *nonstd_glfw = (nonstd_glfw_t *)ptr;
    if (nonstd_glfw->child != NULL && nonstd_glfw->child->cleanup != NULL)
        nonstd_glfw->child->cleanup(nonstd_glfw->child);
    if (nonstd_glfw->sibling != NULL && nonstd_glfw->sibling->cleanup != NULL)
        nonstd_glfw->sibling->cleanup(nonstd_glfw->sibling);
    return 0;
}
int nonstd_glfw_event_handler(void *ptr, void *e)
{
    nonstd_glfw_t *nonstd_glfw = (nonstd_glfw_t *)ptr;

    if (nonstd_glfw->child != NULL && nonstd_glfw->child->event_handler != NULL)
        nonstd_glfw->child->event_handler(nonstd_glfw->child, e);
    if (nonstd_glfw->sibling != NULL && nonstd_glfw->sibling->event_handler != NULL)
        nonstd_glfw->sibling->event_handler(nonstd_glfw->sibling, e);
    return 0;
}
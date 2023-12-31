#include "nonstd_glfw_event.h"
#include "nonstd_glfw_window.h"

int has_glfw_initialized = GL_FALSE;
int has_glew_initialized = GL_FALSE;

int checkContext(nonstd_glfw_window_t *window)
{
    if (glfwGetCurrentContext() != window->window)
    {
        glfwMakeContextCurrent(window->window);
    }
    return 0;
}

int nonstd_glfw_window_init(nonstd_glfw_window_t *window, unsigned int width, unsigned int height, const char *title)
{
    window->base.update = nonstd_glfw_update;
    window->base.draw = nonstd_glfw_window_draw;
    window->base.cleanup = nonstd_glfw_window_cleanup;
    window->base.event_handler = nonstd_glfw_window_event_handler;
    window->base.sibling = NULL;
    window->base.child = NULL;

    window->width = width;
    window->height = height;

    if (has_glfw_initialized != GLFW_TRUE)
    {
        glfwSetErrorCallback(errorcallback);
        // glfw: initialize and configure
        // ------------------------------
        has_glfw_initialized = glfwInit();
        if (GLFW_TRUE != has_glfw_initialized)
        {
            //  fprintf(stderr, "Error WITH GLFW\n");
            return -1;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }

    // glfw window creation
    // --------------------
    window->window = glfwCreateWindow(window->width, window->height, title, NULL, NULL);
    if (window->window == NULL)
    {
        //   fprintf(stderr, "Failed to create GLFW window\n");
        return -1;
    }
    glfwMakeContextCurrent(window->window);
    glfwSetWindowUserPointer(window->window, window);
    install_callbacks(window->window);
    
    if (has_glew_initialized != GL_TRUE)
    {

        // glew: load all OpenGL function pointers
        // ---------------------------------------
        // If using GLEW version 1.13 or earlier
        glewExperimental = GL_TRUE;
        GLenum glew_err = glewInit();
        if (GLEW_OK != glew_err)
        {
            /* Problem: glewInit failed, something is seriously wrong. */
            //  fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_err));
            return -1;
        }
        has_glew_initialized = GL_TRUE;
    }

    return 0;
}

int nonstd_glfw_window_swap(nonstd_glfw_window_t *window)
{
    checkContext(window);
    glfwSwapBuffers(window->window);
    glfwPollEvents();
    return 0;
}

int nonstd_glfw_window_should_close(nonstd_glfw_window_t *window, int *should_close)
{
    checkContext(window);
    *should_close = glfwWindowShouldClose(window->window);
    return 0;
}

int nonstd_glfw_window_draw(void *ptr)
{
    nonstd_glfw_window_t *window = (nonstd_glfw_window_t *)ptr;
    checkContext(window);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    nonstd_glfw_draw(window);
    return 0;
}

int nonstd_glfw_window_cleanup(void *ptr)
{
    nonstd_glfw_window_t *window = (nonstd_glfw_window_t *)ptr;
    checkContext(window);
    glfwSetWindowShouldClose(window->window, GL_TRUE);
    glfwDestroyWindow(window->window);
    nonstd_glfw_cleanup(window);

    glfwMakeContextCurrent(NULL);
    return 0;
}

int nonstd_glfw_window_framebuffersizefun(nonstd_glfw_window_t *window, framebuffersizedata_t *event)
{
    checkContext(window);
    window->width = event->width;
    window->height = event->height;
    glViewport(0, 0, event->width, event->height);
    return 0;
}

int nonstd_glfw_window_keyfun(nonstd_glfw_window_t *window, keydata_t *event)
{
    checkContext(window);
    if (event->key == GLFW_KEY_ESCAPE && event->action == GLFW_PRESS)
    {
        nonstd_glfw_window_cleanup((void*)window);
    }
    return 0;
}

int nonstd_glfw_window_event_handler(void *ptr, void *e)
{
    event_t *event = (event_t *)e;
    nonstd_glfw_window_t *window = (nonstd_glfw_window_t *)ptr;
    switch (event->type)
    {
    case FRAMEBUFFERSIZE:
        nonstd_glfw_window_framebuffersizefun(window, &(event->framebuffersizedata));
        break;
    case KEY:
        nonstd_glfw_window_keyfun(window, &(event->keydata));
        break;
    default:
        break;
    }
    nonstd_glfw_event_handler(window, event);
    return 0;
}

#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "nonstd.h"
#include "nonstd_glfw_window.h"

int has_glfw_initialized = GL_FALSE;
int has_glew_initialized = GL_FALSE;

GLFWerrorfun old_errorcallback;
void errorcallback(int error_code, const char *description)
{
    fprintf(stderr, "GLFW ERROR %d:%s", error_code, description);
    fflush(stderr);
    if (old_errorcallback != NULL)
        old_errorcallback(error_code, description);
    exit(error_code);
};
void window_framebuffersizefun(GLFWwindow *window, int width, int height);

int window_init(window_t *window, unsigned int width, unsigned int height, const char *title, float clear_red, float clear_green, float clear_blue, float clear_alpha)
{
    assert(window != NULL && "NULL WINDOW PTR");
    assert(title != NULL && "NULL TITLE PTR");
    window->width = width;
    window->height = height;
    window->aspect = (float)window->width / (float)window->height;
    glfwSetErrorCallback(errorcallback);

    // glfw: initialize and configure
    // ------------------------------
    has_glfw_initialized = glfwInit();
    if (GLFW_TRUE != has_glfw_initialized)
    {
        fprintf(stderr, "GLFW FAILED TO INITIALIZE at %s:%d %s\n", __FILE__, __LINE__, "glfwInit();");
        fflush(stderr);
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window->window = glfwCreateWindow(window->width, window->height, title, NULL, NULL);
    if (window->window == NULL)
    {
        fprintf(stderr, "GLFW FAILED TO CREATE WINDOW at %s:%d %s\n", __FILE__, __LINE__, "glfwCreateWindow();");
        fflush(stderr);
        return -1;
    }

#ifdef CONTEXT_SWITCHING
    GLFWwindow *oldContext = glfwGetCurrentContext();
#endif
    glfwMakeContextCurrent(window->window);

    // glfwSwapInterval(0); // unlimited
    glfwSwapInterval(1); // vsync

    // glew: load all OpenGL function pointers
    // ---------------------------------------
    // If using GLEW version 1.13 or earlier
    // glewExperimental = GL_TRUE;
    GLenum glew_err = glewInit();
    if (GLEW_OK != glew_err)
    {
        fprintf(stderr, "GLEW FAILED TO INITIALIZE at %s:%d %s\n", __FILE__, __LINE__, glewGetErrorString(glew_err));
        fflush(stderr);
        return -1;
    }

    glClearColor(clear_red, clear_green, clear_blue, clear_alpha);

#ifdef CONTEXT_SWITCHING
    glfwMakeContextCurrent(oldContext);
#endif

    return 0;
}

void window_cleanup(window_t *window)
{
#ifdef CONTEXT_SWITCHING
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(window->window);
#endif
    glfwDestroyWindow(window->window);
#ifdef CONTEXT_SWITCHING
    glfwMakeContextCurrent(oldContext);
#endif
}

void window_clear(window_t *window)
{
#ifdef CONTEXT_SWITCHING
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(window->window);
#else
    if (window == NULL)
        return;
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef CONTEXT_SWITCHING
    glfwMakeContextCurrent(oldContext);
#endif
}

void window_swap(window_t *window)
{
#ifdef CONTEXT_SWITCHING
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(window->window);
#endif
    glfwSwapBuffers(window->window);
#ifdef CONTEXT_SWITCHING
    glfwMakeContextCurrent(oldContext);
#endif
}

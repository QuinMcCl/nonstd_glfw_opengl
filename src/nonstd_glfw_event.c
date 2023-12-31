
#define PRINT_DEBUG
#ifdef PRINT_DEBUG
#include <stdio.h>
#endif
#include "nonstd_glfw.h"
#include "nonstd_glfw_event.h"

GLFWerrorfun old_errorcallback = NULL;
GLFWwindowposfun old_windowposcallback = NULL;
GLFWwindowsizefun old_windowsizecallback = NULL;
GLFWwindowclosefun old_windowclosecallback = NULL;
GLFWwindowrefreshfun old_windowrefreshcallback = NULL;
GLFWwindowfocusfun old_windowfocuscallback = NULL;
GLFWwindowiconifyfun old_windowiconifycallback = NULL;
GLFWwindowmaximizefun old_windowmaximizecallback = NULL;
GLFWframebuffersizefun old_framebuffersizecallback = NULL;
GLFWwindowcontentscalefun old_windowcontentscalecallback = NULL;
GLFWmousebuttonfun old_mousebuttoncallback = NULL;
GLFWcursorposfun old_cursorposcallback = NULL;
GLFWcursorenterfun old_cursorentercallback = NULL;
GLFWscrollfun old_scrollcallback = NULL;
GLFWkeyfun old_keycallback = NULL;
GLFWcharfun old_charcallback = NULL;
GLFWcharmodsfun old_charmodscallback = NULL;
GLFWdropfun old_dropcallback = NULL;
GLFWmonitorfun old_monitorcallback = NULL;
GLFWjoystickfun old_joystickcallback = NULL;

void errorcallback(int error_code, const char *description)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "Error%d:%s\n", error_code, description);
#endif
    if (old_errorcallback != NULL)
        old_errorcallback(error_code, description);
    exit(-1);
};

void windowposcallback(GLFWwindow *window, int xpos, int ypos)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowposcallback:%d:%d\n", xpos, ypos);
#endif
    if (old_windowposcallback != NULL)
        old_windowposcallback(window, xpos, ypos);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = WINDOWPOS, .windowposdata = (windowposdata_t){.window = window, .xpos = xpos, .ypos = ypos}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void windowsizecallback(GLFWwindow *window, int width, int height)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowsizecallback:%d:%d\n", width, height);
#endif
    if (old_windowsizecallback != NULL)
        old_windowsizecallback(window, width, height);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = WINDOWSIZE, .windowsizedata = (windowsizedata_t){.window = window, .width = width, .height = height}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void windowclosecallback(GLFWwindow *window)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowclosecallback\n");
#endif
    if (old_windowclosecallback != NULL)
        old_windowclosecallback(window);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = WINDOWCLOSE, .windowclosedata = (windowclosedata_t){.window = window}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void windowrefreshcallback(GLFWwindow *window)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowrefreshcallback\n");
#endif
    if (old_windowrefreshcallback != NULL)
        old_windowrefreshcallback(window);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = WINDOWREFRESH, .windowrefreshdata = (windowrefreshdata_t){.window = window}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void windowfocuscallback(GLFWwindow *window, int focused)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowfocuscallback:%d\n", focused);
#endif
    if (old_windowfocuscallback != NULL)
        old_windowfocuscallback(window, focused);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = WINDOWFOCUS, .windowfocusdata = (windowfocusdata_t){.window = window, .focused = focused}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void windowiconifycallback(GLFWwindow *window, int iconified)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowiconifycallback:%d\n", iconified);
#endif
    if (old_windowiconifycallback != NULL)
        old_windowiconifycallback(window, iconified);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = WINDOWICONIFY, .windowiconifydata = (windowiconifydata_t){.window = window, .iconified = iconified}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void windowmaximizecallback(GLFWwindow *window, int maximized)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowmaximizecallback:%d\n", maximized);
#endif
    if (old_windowmaximizecallback != NULL)
        old_windowmaximizecallback(window, maximized);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = WINDOWMAXIMIZE, .windowmaximizedata = (windowmaximizedata_t){.window = window, .maximized = maximized}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void framebuffersizecallback(GLFWwindow *window, int width, int height)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "framebuffersizecallback:%d:%d\n", width, height);
#endif
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    if (old_framebuffersizecallback != NULL)
        old_framebuffersizecallback(window, width, height);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = FRAMEBUFFERSIZE, .framebuffersizedata = (framebuffersizedata_t){.window = window, .width = width, .height = height}});

        glfwMakeContextCurrent(oldContext);
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void windowcontentscalecallback(GLFWwindow *window, float xscale, float yscale)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowcontentscalecallback:%f:%f\n", xscale, yscale);
#endif
    if (old_windowcontentscalecallback != NULL)
        old_windowcontentscalecallback(window, xscale, yscale);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = WINDOWCONTENTSCALE, .windowcontentscaledata = (windowcontentscaledata_t){.window = window, .xscale = xscale, .yscale = yscale}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void mousebuttoncallback(GLFWwindow *window, int button, int action, int mods)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "mousebuttoncallback:%d:%d:%d\n", button, action, mods);
#endif
    if (old_mousebuttoncallback != NULL)
        old_mousebuttoncallback(window, button, action, mods);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = MOUSEBUTTON, .mousebuttondata = (mousebuttondata_t){.window = window, .button = button, .action = action, .mods = mods}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void cursorposcallback(GLFWwindow *window, double xpos, double ypos)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "cursorposcallback:%f:%f\n", xpos, ypos);
#endif
    if (old_cursorposcallback != NULL)
        old_cursorposcallback(window, xpos, ypos);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = CURSORPOS, .cursorposdata = (cursorposdata_t){.window = window, .xpos = xpos, .ypos = ypos}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void cursorentercallback(GLFWwindow *window, int entered)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "cursorentercallback:%d\n", entered);
#endif
    if (old_cursorentercallback != NULL)
        old_cursorentercallback(window, entered);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = CURSORENTER, .cursorenterdata = (cursorenterdata_t){.window = window, .entered = entered}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void scrollcallback(GLFWwindow *window, double xoffset, double yoffset)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "scrollcallback:%f:%f\n", xoffset, yoffset);
#endif
    if (old_scrollcallback != NULL)
        old_scrollcallback(window, xoffset, yoffset);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = SCROLL, .scrolldata = (scrolldata_t){.window = window, xoffset = xoffset, .yoffset = yoffset}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void keycallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "keycallback:%d:%d:%d:%d\n", key, scancode, action, mods);
#endif
    if (old_keycallback != NULL)
        old_keycallback(window, key, scancode, action, mods);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = KEY, .keydata = (keydata_t){.window = window, .key = key, .scancode = scancode, .action = action, .mods = mods}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void charcallback(GLFWwindow *window, unsigned int codepoint)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "charcallback:%d\n", codepoint);
#endif
    if (old_charcallback != NULL)
        old_charcallback(window, codepoint);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = CHARPRESS, .charpressdata = (charpressdata_t){.window = window, .codepoint = codepoint}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void charmodscallback(GLFWwindow *window, unsigned int codepoint, int mods)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "charmodscallback:%d:%d\n", codepoint, mods);
#endif
    if (old_charmodscallback != NULL)
        old_charmodscallback(window, codepoint, mods);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = CHARMODS, .charmodsdata = (charmodsdata_t){.window = window, .codepoint = codepoint, .mods = mods}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void dropcallback(GLFWwindow *window, int path_count, const char **paths)
{
    glfwMakeContextCurrent(window);
#ifdef PRINT_DEBUG
    fprintf(stderr, "dropcallback:%d\n", path_count);
    for (int i = 0; i < path_count; i++)
        fprintf(stderr, "%s\n", paths[i]);
#endif
    if (old_dropcallback != NULL)
        old_dropcallback(window, path_count, paths);
    nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    {
        event_t new_event = ((event_t){.type = DROP, .dropdata = (dropdata_t){.window = window, .path_count = path_count, .paths = paths}});
        nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    }
};

void monitorcallback(GLFWmonitor *monitor, int event)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "monitorcallback:%d\n", event);
#endif
    if (old_monitorcallback != NULL)
        old_monitorcallback(monitor, event);
    // nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    // if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    // {
    //     event_t new_event = ((event_t){.type = MONITOR, .monitordata = (monitordata_t){.monitor = monitor, .event = event}});
    //     nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    // }
};

void joystickcallback(int jid, int event)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "joystickcallback:%d:%d\n", jid, event);
#endif
    if (old_charcallback != NULL)
        old_joystickcallback(jid, event);
    // nonstd_glfw_t *nonstd_glfw_root = (nonstd_glfw_t *)glfwGetWindowUserPointer(window);
    // if (nonstd_glfw_root != NULL && nonstd_glfw_root->event_handler != NULL)
    // {
    //     event_t new_event = ((event_t){.type = JOYSTICK, .joystickdata = (joystickdata_t){.jid = jid, .event = event}});
    //     nonstd_glfw_root->event_handler(nonstd_glfw_root, &new_event);
    // }
};

void install_error_callback()
{
    old_errorcallback = glfwSetErrorCallback(errorcallback);
}

void install_callbacks(GLFWwindow *window)
{
    old_windowposcallback = glfwSetWindowPosCallback(window, windowposcallback);
    old_windowsizecallback = glfwSetWindowSizeCallback(window, windowsizecallback);
    old_windowclosecallback = glfwSetWindowCloseCallback(window, windowclosecallback);
    old_windowrefreshcallback = glfwSetWindowRefreshCallback(window, windowrefreshcallback);
    old_windowfocuscallback = glfwSetWindowFocusCallback(window, windowfocuscallback);
    old_windowiconifycallback = glfwSetWindowIconifyCallback(window, windowiconifycallback);
    old_windowmaximizecallback = glfwSetWindowMaximizeCallback(window, windowmaximizecallback);
    old_framebuffersizecallback = glfwSetFramebufferSizeCallback(window, framebuffersizecallback);
    old_windowcontentscalecallback = glfwSetWindowContentScaleCallback(window, windowcontentscalecallback);
    old_mousebuttoncallback = glfwSetMouseButtonCallback(window, mousebuttoncallback);
    old_cursorposcallback = glfwSetCursorPosCallback(window, cursorposcallback);
    old_cursorentercallback = glfwSetCursorEnterCallback(window, cursorentercallback);
    old_scrollcallback = glfwSetScrollCallback(window, scrollcallback);
    old_keycallback = glfwSetKeyCallback(window, keycallback);
    old_charcallback = glfwSetCharCallback(window, charcallback);
    old_charmodscallback = glfwSetCharModsCallback(window, charmodscallback);
    old_dropcallback = glfwSetDropCallback(window, dropcallback);
    old_monitorcallback = glfwSetMonitorCallback(monitorcallback);
    old_joystickcallback = glfwSetJoystickCallback(joystickcallback);
}
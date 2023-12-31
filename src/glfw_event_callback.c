
#ifdef PRINT_DEBUG
#include <stdio.h>
#endif
#include "glfw_event_callback.h"


EventHandler GlobalGLFWEventHandler = NULL;

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
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowposcallback:%d:%d\n", xpos, ypos);
#endif
    if (old_windowposcallback != NULL)
        old_windowposcallback(window, xpos, ypos);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = WINDOWPOS, .windowposdata = (windowposdata_t){.window = window, .xpos = xpos, .ypos = ypos}});
};

void windowsizecallback(GLFWwindow *window, int width, int height)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowsizecallback:%d:%d\n", width, height);
#endif
    if (old_windowsizecallback != NULL)
        old_windowsizecallback(window, width, height);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = WINDOWSIZE, .windowsizedata = (windowsizedata_t){.window = window, .width = width, .height = height}});
};

void windowclosecallback(GLFWwindow *window)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowclosecallback\n");
#endif
    if (old_windowclosecallback != NULL)
        old_windowclosecallback(window);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = WINDOWCLOSE, .windowclosedata = (windowclosedata_t){.window = window}});
};

void windowrefreshcallback(GLFWwindow *window)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowrefreshcallback\n");
#endif
    if (old_windowrefreshcallback != NULL)
        old_windowrefreshcallback(window);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = WINDOWREFRESH, .windowrefreshdata = (windowrefreshdata_t){.window = window}});
};

void windowfocuscallback(GLFWwindow *window, int focused)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowfocuscallback:%d\n", focused);
#endif
    if (old_windowfocuscallback != NULL)
        old_windowfocuscallback(window, focused);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = WINDOWFOCUS, .windowfocusdata = (windowfocusdata_t){.window = window, .focused = focused}});
};

void windowiconifycallback(GLFWwindow *window, int iconified)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowiconifycallback:%d\n", iconified);
#endif
    if (old_windowiconifycallback != NULL)
        old_windowiconifycallback(window, iconified);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = WINDOWICONIFY, .windowiconifydata = (windowiconifydata_t){.window = window, .iconified = iconified}});
};

void windowmaximizecallback(GLFWwindow *window, int maximized)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowmaximizecallback:%d\n", maximized);
#endif
    if (old_windowmaximizecallback != NULL)
        old_windowmaximizecallback(window, maximized);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = WINDOWMAXIMIZE, .windowmaximizedata = (windowmaximizedata_t){.window = window, .maximized = maximized}});
};

void framebuffersizecallback(GLFWwindow *window, int width, int height)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "framebuffersizecallback:%d:%d\n", width, height);
#endif
    GLFWwindow *oldContext = glfwGetCurrentContext();
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    if (old_framebuffersizecallback != NULL)
        old_framebuffersizecallback(window, width, height);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = FRAMEBUFFERSIZE, .framebuffersizedata = (framebuffersizedata_t){.window = window, .width = width, .height = height}});

    glfwMakeContextCurrent(oldContext);
};

void windowcontentscalecallback(GLFWwindow *window, float xscale, float yscale)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "windowcontentscalecallback:%f:%f\n", xscale, yscale);
#endif
    if (old_windowcontentscalecallback != NULL)
        old_windowcontentscalecallback(window, xscale, yscale);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = WINDOWCONTENTSCALE, .windowcontentscaledata = (windowcontentscaledata_t){.window = window, .xscale = xscale, .yscale = yscale}});
};

void mousebuttoncallback(GLFWwindow *window, int button, int action, int mods)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "mousebuttoncallback:%d:%d:%d\n", button, action, mods);
#endif
    if (old_mousebuttoncallback != NULL)
        old_mousebuttoncallback(window, button, action, mods);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = MOUSEBUTTON, .mousebuttondata = (mousebuttondata_t){.window = window, .button = button, .action = action, .mods = mods}});
};

void cursorposcallback(GLFWwindow *window, double xpos, double ypos)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "cursorposcallback:%f:%f\n", xpos, ypos);
#endif
    if (old_cursorposcallback != NULL)
        old_cursorposcallback(window, xpos, ypos);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = CURSORPOS, .cursorposdata = (cursorposdata_t){.window = window, .xpos = xpos, .ypos = ypos}});
};

void cursorentercallback(GLFWwindow *window, int entered)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "cursorentercallback:%d\n", entered);
#endif
    if (old_cursorentercallback != NULL)
        old_cursorentercallback(window, entered);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = CURSORENTER, .cursorenterdata = (cursorenterdata_t){.window = window, .entered = entered}});
};

void scrollcallback(GLFWwindow *window, double xoffset, double yoffset)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "scrollcallback:%f:%f\n", xoffset, yoffset);
#endif
    if (old_scrollcallback != NULL)
        old_scrollcallback(window, xoffset, yoffset);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = SCROLL, .scrolldata = (scrolldata_t){.window = window, xoffset = xoffset, .yoffset = yoffset}});
};

void keycallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "keycallback:%d:%d:%d:%d\n", key, scancode, action, mods);
#endif
    if (old_keycallback != NULL)
        old_keycallback(window, key, scancode, action, mods);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = KEY, .keydata = (keydata_t){.window = window, .key = key, .scancode = scancode, .action = action, .mods = mods}});
};

void charcallback(GLFWwindow *window, unsigned int codepoint)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "charcallback:%d\n", codepoint);
#endif
    if (old_charcallback != NULL)
        old_charcallback(window, codepoint);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = CHARPRESS, .charpressdata = (charpressdata_t){.window = window, .codepoint = codepoint}});
};

void charmodscallback(GLFWwindow *window, unsigned int codepoint, int mods)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "charmodscallback:%d:%d\n", codepoint, mods);
#endif
    if (old_charmodscallback != NULL)
        old_charmodscallback(window, codepoint, mods);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = CHARMODS, .charmodsdata = (charmodsdata_t){.window = window, .codepoint = codepoint, .mods = mods}});
};

void dropcallback(GLFWwindow *window, int path_count, const char **paths)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "dropcallback:%d\n", path_count);
    for (int i = 0; i < path_count; i++)
        fprintf(stderr, "%s\n", paths[i]);
#endif
    if (old_dropcallback != NULL)
        old_dropcallback(window, path_count, paths);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = DROP, .dropdata = (dropdata_t){.window = window, .path_count = path_count, .paths = paths}});
};

void monitorcallback(GLFWmonitor *monitor, int event)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "monitorcallback:%d\n", event);
#endif
    if (old_monitorcallback != NULL)
        old_monitorcallback(monitor, event);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = MONITOR, .monitordata = (monitordata_t){.monitor = monitor, .event = event}});
};

void joystickcallback(int jid, int event)
{
#ifdef PRINT_DEBUG
    fprintf(stderr, "joystickcallback:%d:%d\n", jid, event);
#endif
    if (old_charcallback != NULL)
        old_joystickcallback(jid, event);
    if (GlobalGLFWEventHandler != NULL)
        GlobalGLFWEventHandler((event_t){.type = JOYSTICK, .joystickdata = (joystickdata_t){.jid = jid, .event = event}});
};

void install_error_callback()
{
    old_errorcallback = glfwSetErrorCallback(errorcallback);
}

void install_callbacks(GLFWwindow *window, EventHandler newHandler)
{
    GlobalGLFWEventHandler = newHandler;
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
#ifndef NONSTD_GLFW_EVENT_H
#define NONSTD_GLFW_EVENT_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include "nonstd_glfw.h"

void errorcallback(int error_code, const char *description);

void windowposcallback(GLFWwindow *window, int xpos, int ypos);
void windowsizecallback(GLFWwindow *window, int width, int height);
void windowclosecallback(GLFWwindow *window);
void windowrefreshcallback(GLFWwindow *window);
void windowfocuscallback(GLFWwindow *window, int focused);
void windowiconifycallback(GLFWwindow *window, int iconified);
void windowmaximizecallback(GLFWwindow *window, int maximized);
void framebuffersizecallback(GLFWwindow *window, int width, int height);
void windowcontentscalecallback(GLFWwindow *window, float xscale, float yscale);
void mousebuttoncallback(GLFWwindow *window, int button, int action, int mods);
void cursorposcallback(GLFWwindow *window, double xpos, double ypos);
void cursorentercallback(GLFWwindow *window, int entered);
void scrollcallback(GLFWwindow *window, double xoffset, double yoffset);
void keycallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void charcallback(GLFWwindow *window, unsigned int codepoint);
void charmodscallback(GLFWwindow *window, unsigned int codepoint, int mods);
void dropcallback(GLFWwindow *window, int path_count, const char **paths);
void monitorcallback(GLFWmonitor *monitor, int event);
void joystickcallback(int jid, int event);

typedef enum callbackType_e
{
    WINDOWPOS,
    WINDOWSIZE,
    WINDOWCLOSE,
    WINDOWREFRESH,
    WINDOWFOCUS,
    WINDOWICONIFY,
    WINDOWMAXIMIZE,
    FRAMEBUFFERSIZE,
    WINDOWCONTENTSCALE,
    MOUSEBUTTON,
    CURSORPOS,
    CURSORENTER,
    SCROLL,
    KEY,
    CHARPRESS,
    CHARMODS,
    DROP,
    MONITOR,
    JOYSTICK,

} callbackType_t;

typedef struct windowposdata_s
{
    GLFWwindow *window;
    int xpos;
    int ypos;
} windowposdata_t;
typedef struct windowsizedata_s
{
    GLFWwindow *window;
    int width;
    int height;
} windowsizedata_t;
typedef struct windowclosedata_s
{
    GLFWwindow *window;
} windowclosedata_t;
typedef struct windowrefreshdata_s
{
    GLFWwindow *window;
} windowrefreshdata_t;
typedef struct windowfocusdata_s
{
    GLFWwindow *window;
    int focused;
} windowfocusdata_t;
typedef struct windowiconifydata_s
{
    GLFWwindow *window;
    int iconified;
} windowiconifydata_t;
typedef struct windowmaximizedata_s
{
    GLFWwindow *window;
    int maximized;
} windowmaximizedata_t;
typedef struct framebuffersizedata_s
{
    GLFWwindow *window;
    int width;
    int height;
} framebuffersizedata_t;
typedef struct windowcontentscaledata_s
{
    GLFWwindow *window;
    float xscale;
    float yscale;
} windowcontentscaledata_t;
typedef struct mousebuttondata_s
{
    GLFWwindow *window;
    int button;
    int action;
    int mods;
} mousebuttondata_t;
typedef struct cursorposdata_s
{
    GLFWwindow *window;
    double xpos;
    double ypos;
} cursorposdata_t;
typedef struct cursorenterdata_s
{
    GLFWwindow *window;
    int entered;
} cursorenterdata_t;
typedef struct scrolldata_s
{
    GLFWwindow *window;
    double xoffset;
    double yoffset;
} scrolldata_t;
typedef struct keydata_s
{
    GLFWwindow *window;
    int key;
    int scancode;
    int action;
    int mods;
} keydata_t;
typedef struct charpressdata_s
{
    GLFWwindow *window;
    unsigned int codepoint;
} charpressdata_t;
typedef struct charmodsdata_s
{
    GLFWwindow *window;
    unsigned int codepoint;
    int mods;
} charmodsdata_t;
typedef struct dropdata_s
{
    GLFWwindow *window;
    int path_count;
    const char **paths;
} dropdata_t;
typedef struct monitordata_s
{
    GLFWmonitor *monitor;
    int event;
} monitordata_t;
typedef struct joystickdata_s
{
    int jid;
    int event;
} joystickdata_t;

typedef struct event_s
{
    callbackType_t type;
    union
    {
        windowposdata_t windowposdata;
        windowsizedata_t windowsizedata;
        windowclosedata_t windowclosedata;
        windowrefreshdata_t windowrefreshdata;
        windowfocusdata_t windowfocusdata;
        windowiconifydata_t windowiconifydata;
        windowmaximizedata_t windowmaximizedata;
        framebuffersizedata_t framebuffersizedata;
        windowcontentscaledata_t windowcontentscaledata;
        mousebuttondata_t mousebuttondata;
        cursorposdata_t cursorposdata;
        cursorenterdata_t cursorenterdata;
        scrolldata_t scrolldata;
        keydata_t keydata;
        charpressdata_t charpressdata;
        charmodsdata_t charmodsdata;
        dropdata_t dropdata;
        monitordata_t monitordata;
        joystickdata_t joystickdata;
    };
} event_t;

void install_callbacks(GLFWwindow *window, nonstd_glfw_t *node);

#endif
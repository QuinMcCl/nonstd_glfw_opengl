#ifndef NONSTD_GLFW_CAMERA_H
#define NONSTD_GLFW_CAMERA_H

#include <cglm/cglm.h>
#include "nonstd_glfw.h"
#include "nonstd_opengl_ubo.h"
#include "nonstd_glfw_event.h"

typedef enum Camera_Movement_e
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    MAX_CAMERA_MOVEMENT
} Camera_Movement_t;

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

const int default_inputs[] = {
    GLFW_KEY_W,
    GLFW_KEY_S,
    GLFW_KEY_A,
    GLFW_KEY_D,
    GLFW_KEY_SPACE,
    GLFW_KEY_LEFT_CONTROL,
};

typedef struct nonstd_glfw_camera_s
{
    nonstd_glfw_t base;
    int is_active;
    // camera vectors
    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float aspect;

    int Inputs[MAX_CAMERA_MOVEMENT];
    int Input_Key[MAX_CAMERA_MOVEMENT];
    int firstMouse;
    int LeftMouseDown;
    int constrainPitch;
    double lastX;
    double lastY;

    nonstd_opengl_ubo_t cameraViewProjection;

} nonstd_glfw_camera_t;

int nonstd_glfw_camera_init(nonstd_glfw_camera_t *camera,
                            const int active,
                            const vec3 position,
                            const vec3 up,
                            const float yaw,
                            const float pitch,
                            const float aspect);

int nonstd_glfw_camera_cleanup(void *ptr);
int nonstd_glfw_camera_update(void *ptr, double dt);
int nonstd_glfw_camera_draw(void *ptr);
int nonstd_glfw_camera_event_handler(void *ptr, void *e);

#endif
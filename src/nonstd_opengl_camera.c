
#include <math.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include "nonstd_glfw_event.h"
#include "nonstd_opengl_ubo.h"
#include "nonstd_glfw_camera.h"

int nonstd_glfw_camera_init(nonstd_glfw_camera_t *camera,
                            const int active,
                            const vec3 position,
                            const vec3 up,
                            const float yaw,
                            const float pitch,
                            const float aspect)
{
    camera->base.update = nonstd_glfw_camera_update;
    camera->base.draw = nonstd_glfw_camera_draw;
    camera->base.cleanup = nonstd_glfw_camera_cleanup;
    camera->base.event_handler = nonstd_glfw_camera_event_handler;
    camera->base.sibling = NULL;
    camera->base.child = NULL;

    nonstd_opengl_ubo_init(&(camera->cameraViewProjection), "uboViewProjection", 2 * sizeof(mat4), GL_STREAM_DRAW);

    camera->is_active = active;
    memcpy(camera->pos, position, sizeof(vec3));
    memcpy(camera->WorldUp, up, sizeof(vec3));
    camera->Pitch = pitch;
    camera->Yaw = yaw;

    camera->MovementSpeed = SPEED;
    camera->MouseSensitivity = SENSITIVITY;
    camera->Zoom = ZOOM;
    camera->aspect = aspect;

    camera->firstMouse = 1;
    camera->LeftMouseDown = GLFW_RELEASE;
    camera->constrainPitch = 1;

    for (int index = 0; index < MAX_CAMERA_MOVEMENT; index++)
    {
        camera->Inputs[index] = GLFW_RELEASE;
        camera->Input_Key[index] = default_inputs[index];
    }

    return 0;
}

int nonstd_glfw_camera_cleanup(void *ptr)
{
    nonstd_glfw_cleanup(ptr);
    return 0;
}

int nonstd_glfw_camera_update(void *ptr, double dt)
{
    nonstd_glfw_camera_t *camera = (nonstd_glfw_camera_t *)ptr;
    vec3 displacement;
    float velocity = camera->MovementSpeed * (float)dt;
    if (camera->Inputs[FORWARD] == GLFW_PRESS)
    {
        glm_vec3_scale(camera->front, velocity, displacement);
        glm_vec3_add(camera->pos, displacement, camera->pos);
    }
    if (camera->Inputs[BACKWARD] == GLFW_PRESS)
    {
        glm_vec3_scale(camera->front, -1.0f * velocity, displacement);
        glm_vec3_add(camera->pos, displacement, camera->pos);
    }
    if (camera->Inputs[LEFT] == GLFW_PRESS)
    {
        glm_vec3_scale(camera->right, -1.0f * velocity, displacement);
        glm_vec3_add(camera->pos, displacement, camera->pos);
    }
    if (camera->Inputs[RIGHT] == GLFW_PRESS)
    {
        glm_vec3_scale(camera->right, velocity, displacement);
        glm_vec3_add(camera->pos, displacement, camera->pos);
    }
    if (camera->Inputs[UP] == GLFW_PRESS)
    {
        glm_vec3_scale(camera->up, velocity, displacement);
        glm_vec3_add(camera->pos, displacement, camera->pos);
    }
    if (camera->Inputs[DOWN] == GLFW_PRESS)
    {
        glm_vec3_scale(camera->up, -1.0f * velocity, displacement);
        glm_vec3_add(camera->pos, displacement, camera->pos);
    }

    // calculate the new Front vector
    camera->front[0] = cos(glm_rad(camera->Yaw)) * cos(glm_rad(camera->Pitch));
    camera->front[1] = sin(glm_rad(camera->Pitch));
    camera->front[2] = sin(glm_rad(camera->Yaw)) * cos(glm_rad(camera->Pitch));
    glm_vec3_normalize(camera->front);
    // also re-calculate the Right and Up vector
    glm_vec3_cross(camera->front, camera->WorldUp, camera->right);
    glm_vec3_normalize(camera->right);

    glm_vec3_cross(camera->right, camera->front, camera->up);
    glm_vec3_normalize(camera->up);

    mat4 view_projection[2];
    vec3 dir;
    glm_vec3_add(camera->pos, camera->front, dir);
    glm_look(camera->pos, dir, camera->up, view_projection[0]);
    glm_perspective(glm_rad(camera->Zoom), camera->aspect, 0.1f, 100.0f, view_projection[1]);
    nonstd_opengl_ubo_fill(&(camera->cameraViewProjection), view_projection, sizeof(view_projection), 0);

    nonstd_glfw_update(ptr, dt);
    return 0;
}

int nonstd_glfw_camera_draw(void *ptr)
{
    nonstd_glfw_draw(ptr);
    return 0;
}

int nonstd_glfw_camera_mouse_button_func(nonstd_glfw_camera_t *camera, mousebuttondata_t *data)
{
    if (data->action == GLFW_REPEAT)
        return 0;
    if (data->button == GLFW_MOUSE_BUTTON_LEFT)
    {
        camera->LeftMouseDown = data->action;
    }
    return 0;
}

int nonstd_glfw_camera_cursor_pos_func(nonstd_glfw_camera_t *camera, cursorposdata_t *data)
{
    if (camera->firstMouse)
    {
        camera->lastX = data->xpos;
        camera->lastY = data->ypos;
        camera->firstMouse = 0;
    }

    float xoffset = data->xpos - camera->lastX;
    float yoffset = camera->lastY - data->ypos; // reversed since y-coordinates go from bottom to top

    camera->lastX = data->xpos;
    camera->lastY = data->ypos;

    if (camera->LeftMouseDown == GLFW_PRESS)
    {

        xoffset *= camera->MouseSensitivity;
        yoffset *= camera->MouseSensitivity;

        camera->Yaw += xoffset;
        camera->Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (camera->constrainPitch)
        {
            if (camera->Pitch > 89.0f)
                camera->Pitch = 89.0f;
            if (camera->Pitch < -89.0f)
                camera->Pitch = -89.0f;
        }
    }
    return 0;
}

int nonstd_glfw_camera_scroll_func(nonstd_glfw_camera_t *camera, scrolldata_t *data)
{

    camera->Zoom -= (float)data->yoffset;
    if (camera->Zoom < 1.0f)
        camera->Zoom = 1.0f;
    if (camera->Zoom > 45.0f)
        camera->Zoom = 45.0f;
    return 0;
}

int nonstd_glfw_camera_key_func(nonstd_glfw_camera_t *camera, keydata_t *data)
{
    if (data->action == GLFW_REPEAT)
        return 0;
    for (unsigned int index = 0; index < MAX_CAMERA_MOVEMENT; index++)
    {
        if (camera->Input_Key[index] == data->key)
        {
            camera->Inputs[index] = data->action;
        }
    }
    return 0;
}

int nonstd_glfw_camera_event_handler(void *ptr, void *e)
{

    nonstd_glfw_camera_t *camera = (nonstd_glfw_camera_t *)ptr;
    event_t *event = (event_t *)e;

    switch (event->type)
    {
    case MOUSEBUTTON:
        nonstd_glfw_camera_mouse_button_func(camera, &(event->mousebuttondata));
        break;
    case CURSORPOS:
        nonstd_glfw_camera_cursor_pos_func(camera, &(event->cursorposdata));
        break;
    case SCROLL:
        nonstd_glfw_camera_scroll_func(camera, &(event->scrolldata));
        break;
    case KEY:
        nonstd_glfw_camera_key_func(camera, &(event->keydata));
        break;

    default:
        break;
    }

    nonstd_glfw_event_handler(ptr, e);
    return 0;
}

#ifndef OPENGL_CAMERA_H
#define OPENGL_CAMERA_H

#include <cglm/cglm.h>
#include "nonstd_opengl_buffer.h"

typedef enum camera_projection_e
{
    PERSPECTIVE,
    ORTHOGRAPHIC
}camera_projection_t;

typedef struct camera_s
{
    vec3 mPosition;
    vec3 mWorldUp;
    float mPitch;
    float mYaw;
    float mRoll;
    int mConstrainPitch;
    camera_projection_t projection_type;
    float mNearZ;
    float mFarZ;
    float mAspect;
    float mFOV;
    nonstd_opengl_ubo_t mViewProjection;
}camera_t;


int camera_alloc(
    camera_t *camera,
    vec3 position,
    vec3 up,
    const float yaw,
    const float pitch,
    const float roll,
    const float nearz,
    const float farz,
    const float aspect,
    const float fov);

int camera_free(camera_t *camera);
int camera_update_view_projection(camera_t *camera);

#endif
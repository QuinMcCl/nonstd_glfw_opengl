#include <GL/glew.h>
#include <cglm/cglm.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "nonstd.h"
#include "nonstd_opengl_buffer.h"
#include "nonstd_opengl_camera.h"

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
    const float fov)
{
    camera->mYaw = yaw;
    camera->mPitch = pitch;
    camera->mRoll = roll;
    camera->mNearZ = nearz;
    camera->mFarZ = farz;
    camera->mAspect = aspect;
    camera->mFOV = fov;
    camera->projection_type = PERSPECTIVE;
    camera->mMouseSensitivity = 0.01f;

    glm_vec3_copy(position, camera->mPosition);
    glm_vec3_copy(up, camera->mWorldUp);

    CHECK_ERR(nonstd_opengl_ubo_init(&(camera->mViewProjection), "uboViewProjection", 2 * sizeof(mat4), GL_STREAM_DRAW), strerror(errno), return errno);
    CHECK_ERR(nonstd_opengl_ubo_init(&(camera->mViewPosition), "uboViewPosition", sizeof(vec3), GL_STREAM_DRAW), strerror(errno), return errno);

    return 0;
}

int camera_free(camera_t *camera)
{
    CHECK_ERR(nonstd_opengl_ubo_cleanup(&(camera->mViewProjection)), strerror(errno), return errno);
    CHECK_ERR(nonstd_opengl_ubo_cleanup(&(camera->mViewPosition)), strerror(errno), return errno);
    return 0;
}

int camera_update_view_projection(camera_t *camera)
{
    vec3 center;
    CHECK_ERR(nonstd_opengl_ubo_fill(&(camera->mViewPosition), camera->mPosition, sizeof(vec3), 0), strerror(errno), return errno);

    camera->front[0] = cos(glm_rad(camera->mYaw)) * cos(glm_rad(camera->mPitch));
    camera->front[1] = sin(glm_rad(camera->mPitch));
    camera->front[2] = sin(glm_rad(camera->mYaw)) * cos(glm_rad(camera->mPitch));

    glm_vec3_normalize(camera->front);
    glm_vec3_add(camera->mPosition, camera->front, center);

    glm_vec3_copy(camera->mWorldUp, camera->up);
    glm_vec3_rotate(camera->up, glm_rad(camera->mRoll), camera->front);
    glm_lookat(camera->mPosition, center, camera->up, camera->mView);

    CHECK_ERR(nonstd_opengl_ubo_fill(&(camera->mViewProjection), camera->mView, sizeof(mat4), 0 * sizeof(mat4)), strerror(errno), return errno);

    if (camera->projection_type == PERSPECTIVE)
    {
        glm_perspective(glm_rad(camera->mFOV), camera->mAspect, camera->mNearZ, camera->mFarZ, camera->mProjection);
    }
    else
    {
        glm_ortho(-camera->mFOV, camera->mFOV, -camera->mFOV / camera->mAspect, camera->mFOV / camera->mAspect, camera->mNearZ, camera->mFarZ, camera->mProjection);
    }

    CHECK_ERR(nonstd_opengl_ubo_fill(&(camera->mViewProjection), camera->mProjection, sizeof(mat4), 1 * sizeof(mat4)), strerror(errno), return errno);

    return 0;
}
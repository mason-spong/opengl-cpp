#include "Camera.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/trigonometric.hpp"

Camera::Camera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 upVec, float fovDeg, float aspect, float near, float far)
    : position(pos), target(lookAt), up(upVec), fov(fovDeg), aspectRatio(aspect), nearPlane(near), farPlane(far) {}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}
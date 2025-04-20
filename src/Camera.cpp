#include "Camera.h"

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

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

// Represents the camera's view and projection
class Camera
{
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    Camera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 upVec, float fovDeg, float aspect, float near, float far);

    glm::mat4 getViewMatrix() const;

    glm::mat4 getProjectionMatrix() const;
};

#endif
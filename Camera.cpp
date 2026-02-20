#include "Camera.h"

Camera::Camera(glm::vec3 position)
{
    Position = position;
}

const glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + FRONT, UP);
}

void Camera::updatePosition(float xoffset, float yoffset, float zoffset)
{
    Position+=xoffset*RIGHT+yoffset*UP+zoffset*FRONT;
}

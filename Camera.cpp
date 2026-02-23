#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MouseSensitivity(0.15f)
{
    Position=position;
    WorldUp=up;
    Yaw=yaw;
    Pitch=pitch;

    updateCameraVectors();
}

const glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position+Front, Up);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    xoffset*=MouseSensitivity;
    yoffset*=MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Prevent flipping
    if (Pitch > 89.9f)  Pitch = 89.9f;
    if (Pitch < -89.9f) Pitch = -89.9f;

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x=cos(glm::radians(Yaw))*cos(glm::radians(Pitch));
    front.y=sin(glm::radians(Pitch));
    front.z=sin(glm::radians(Yaw))*cos(glm::radians(Pitch));

    FlatFront=glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    Front=glm::normalize(front);
    Right=glm::normalize(glm::cross(Front, WorldUp));
    Up=glm::normalize(glm::cross(Right, Front));
}

void Camera::updatePosition(float xpos, float ypos, float zpos)
{
    Position+=xpos*Right+ypos*WorldUp+zpos*FlatFront;
}
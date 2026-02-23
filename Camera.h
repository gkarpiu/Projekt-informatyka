#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera{
    public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::vec3 FlatFront;

    float Yaw;
    float Pitch;

    float MouseSensitivity;

    Camera(glm::vec3 position=glm::vec3(0.0f, 1.5f, 3.0f), glm::vec3 up=glm::vec3(0.0f, 1.0f, 0.0f), float yaw=-90.0f, float pitch=0.0f);

    const glm::mat4 GetViewMatrix();

    void ProcessMouseMovement(float xoffset, float yoffset);

    void updateCameraVectors();

    void updatePosition(float xpos, float ypos, float zpos);
};
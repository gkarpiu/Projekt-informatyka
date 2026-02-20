#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr glm::vec3 FRONT={0.0f, 0.0f, -1.0f};
constexpr glm::vec3 UP={0.0f, 1.0f, 0.0f};
constexpr glm::vec3 RIGHT={1.0f, 0.0f, 0.0f};

class Camera{
    public:
    glm::vec3 Position;

    Camera(glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f));

    const glm::mat4 GetViewMatrix();

    void updatePosition(float xoffset, float yoffset, float zoffset);
};

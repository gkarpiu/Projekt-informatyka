#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Camera.h"

struct Triangle
{
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;
    glm::vec3 normal;

    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
};
struct AABB{
    glm::vec3 position;
    glm::vec3 extents;
};

extern std::vector<Triangle> collisions;
extern const float PLAYER_SPEED;
extern const AABB playerHitbox;

void DoMovement(Camera& camera, GLFWwindow* window);

bool TakeInput(GLFWwindow* window, float& x, float& y, float& z);

bool TestIntersect(const Triangle triangle, const AABB& aabb);
bool testSAT(Triangle triangle, const AABB& aabb, glm::vec3 axis);
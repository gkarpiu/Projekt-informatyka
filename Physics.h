#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include "Engine.h"

struct AABB{
    glm::vec3 position;
    glm::vec3 extents;
};

extern const float PLAYER_SPEED;
extern const AABB playerHitbox;

void DoMovement(Camera& camera, GLFWwindow* window);

bool TakeInput(GLFWwindow* window, float& x, float& y, float& z);

bool TestIntersect(const Triangle triangle, const AABB& aabb);
bool testSAT(Triangle triangle, const AABB& aabb, glm::vec3 axis);
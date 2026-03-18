#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include "Engine.h"

extern const float playerAcceleration;
extern const glm::vec3 gravity;
extern glm::vec3 playerVelocity;
extern const AABB playerDefaultHitbox;
extern const AABB playerCrouchHitbox;
extern AABB playerHitbox;
extern const glm::vec3 spawnPoint;
extern std::vector<Triangle> lastCollisions;
extern bool isOnGround;

void DoMovement(Camera& camera, GLFWwindow* window);

bool TakeInput(GLFWwindow* window, glm::vec3& offset);
bool TestIntersect(Triangle triangle, const AABB& aabb);
void CheckCollision(const AABB localPlayer, const Node* node, glm::vec3& velocity);
bool testSAT(Triangle triangle, const AABB& aabb, glm::vec3 axis);

#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include "Engine.h"

struct AABB{
    glm::vec3 position;
    glm::vec3 extents;
};

extern const float PLAYER_SPEED;
extern const glm::vec3 gravity;
extern const AABB playerHitbox;
extern const glm::vec3 spawnPoint;

void DoMovement(Camera& camera, GLFWwindow* window);

bool TakeInput(GLFWwindow* window, float& x, float& y, float& z);

bool TestIntersect(const Triangle triangle, const AABB& aabb, glm::vec3 playerPos);
void ResolveCollision(Entity& entity, glm::vec3& velocity, Camera& camera);
bool CheckCollision(Entity& entity, Camera& camera);
void CheckTriggers(Camera& camera, std::vector<size_t>& ids);
bool testSAT(Triangle triangle, const AABB& aabb, glm::vec3 axis);
#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Camera.h"

constexpr float PLAYER_SPEED=0.01f;

void DoMovement(Camera& camera, GLFWwindow* window);
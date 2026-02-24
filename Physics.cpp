#include "Physics.h"

void DoMovement(Camera& camera, GLFWwindow* window)
{
    float xoffset=0, yoffset=0, zoffset=0;
    bool moved=0;
    if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS) {
        xoffset+=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS) {
        xoffset-=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS) {
        yoffset-=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS) {
        yoffset+=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS) {
        zoffset-=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS) {
        zoffset+=PLAYER_SPEED;
        moved=1;
    }
    if(moved) camera.updatePosition(xoffset, yoffset, zoffset);
}
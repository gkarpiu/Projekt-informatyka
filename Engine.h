#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include "Camera.h"

constexpr int WINDOW_WIDTH=1920;
constexpr int WINDOW_HEIGHT=1080;

extern const char* worldvertexShaderSource;
extern const char* worldfragmentShaderSource;

extern const char* uivertexShaderSource;
extern const char* uifragmentShaderSource;

struct Triangle{
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;
    glm::vec3 normal;

    Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
};
struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    float light;
    float alpha;
};
struct Renderer{
    GLuint VAO=0;
    GLuint VBO=0;
    GLuint EBO=0;

    size_t idCount;
};
struct Mesh{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
struct Texture{
    GLuint id;
    int width;
    int height;
};
struct Entity{
    glm::mat4 transform;
    size_t mesh;
    size_t hitbox;
    size_t texture;
    bool trigger;
};

extern int WINDOW_SCALE;
extern GLFWwindow* window;
extern unsigned int modelLoc;
extern unsigned int viewLoc;
extern unsigned int worldprojLoc;
extern unsigned int uiprojLoc;
extern unsigned int worldshaderProgram;
extern unsigned int uishaderProgram;
extern bool firstMouse;
extern float lastX, lastY;
extern std::vector<Renderer> meshes;
extern std::vector<Entity> entities;
extern std::vector<Texture> textures;
extern std::vector<std::vector<Triangle>> collisions;
extern glm::mat4 worldprojection;
extern glm::mat4 uiprojection;

extern Camera camera;

size_t LoadTexture(const char* path);
int LoadObject(std::string name);
void UploadMesh(Mesh& mesh, Renderer& renderer);
size_t AddEntity(size_t mesh, size_t hitbox, size_t texture, bool trigger);
void DrawEntity(Entity& entity);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);

int InitEngine();

bool ShouldClose();

void DoDrawing(Camera& camera);

void CloseEngine();

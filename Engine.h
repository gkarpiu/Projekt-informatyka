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
#include <algorithm>
#include <chrono>
#include <thread>
#include "Camera.h"

constexpr int WINDOW_WIDTH=1920;
constexpr int WINDOW_HEIGHT=1080;

extern const char* worldvertexShaderSource;
extern const char* worldfragmentShaderSource;

extern const char* uivertexShaderSource;
extern const char* uifragmentShaderSource;

extern const uint8_t LEAF_SIZE;

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

    size_t texture;

    size_t idCount;
};
struct MeshPart{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    size_t texture;
};
struct Mesh{
    std::vector<MeshPart> parts;
};
struct Texture{
    GLuint id;
    int width;
    int height;
};
struct AABB{
    glm::vec3 position;
    glm::vec3 extents;
};
struct Node{
    AABB aabb;
    Node* l;
    Node* r;
    int start;
    int count;
    int collisionId;
    bool isLeaf;
};
struct Entity{
    glm::mat4 transform;
    std::vector<size_t> mesh;
    Node* bvh;
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

Node* CreateBVH(const std::vector<Triangle>& prims, int start, int end, size_t collisionId);
const glm::vec3 Centroid(const Triangle& a);
bool CollisionAABB(const AABB& a, const AABB& b);
AABB computeBounds(const std::vector<Triangle>& prims, int start, int end);
size_t LoadTexture(const char* path);
Node* LoadObject(std::string name, std::vector<size_t>& meshVec);
void UploadMesh(MeshPart& mesh, Renderer& renderer);
size_t AddEntity(std::vector<size_t>& mesh, Node* hitbox, bool trigger, bool ui);
void DrawEntity(Entity& entity);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);

int InitEngine();

bool ShouldClose();

void DoDrawing(Camera& camera);

void CloseEngine();

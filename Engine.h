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
#include "Camera.h"

constexpr int WINDOW_WIDTH=1920;
constexpr int WINDOW_HEIGHT=1080;

extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    float light;
    float alpha;
};
struct Model{
    std::vector<Vertex> vertices;
};
struct Renderer{
    GLuint VAO=0;
    GLuint VBO=0;
    GLuint EBO=0;
};
struct Object{
    uint32_t indexoffset;
    std::vector<uint32_t> indices; 
    bool alive=1;
};
struct FreePart{
    size_t id;
    size_t size;
};
struct Mesh{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<FreePart> free;
    std::vector<Object> objects;

    bool dirty;

    Renderer renderer;
};
struct Texture{
    GLuint id;
    int width;
    int height;
};

extern int WINDOW_SCALE;
extern GLFWwindow* window;
extern Mesh mesh;
extern unsigned int modelLoc;
extern unsigned int viewLoc;
extern unsigned int projLoc;
extern glm::ivec2 awh;
extern unsigned int shaderProgram;
extern bool firstMouse;
extern float lastX, lastY;

extern Camera camera;

Texture LoadTexture(const char* path);
bool FitsInFree(Model& model, uint32_t& id);
void AddModel(Model& model);
void RemoveObject(Object& object);
void UploadMesh();
void BuildIndexBuffer();
void DrawMesh();
void MouseCallback(GLFWwindow* window, Camera& camera, double xpos, double ypos);

int InitEngine();

bool ShouldClose();

void DoDrawing(Camera& camera);

void CloseEngine();

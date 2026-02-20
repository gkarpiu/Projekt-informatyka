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

constexpr int WINDOW_WIDTH=640;
constexpr int WINDOW_HEIGHT=360;

extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

struct Sprite{
    glm::vec2 uv0;
    glm::vec2 uv1;
};
struct Quad{
    glm::vec3 position;
    glm::vec2 size;
    Sprite sprite;
    float light;
    float alpha;
};
struct Vertex{
    glm::vec3 position;
    glm::vec2 uv;
    float light;
    float alpha;
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
    float z;
};
struct Mesh{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<uint32_t> free;
    std::vector<Object> objects;

    uint32_t indexoffset=0;
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

Texture LoadTexture(const char* path);
void AddObject(Object& object);
Sprite MakeSprite(glm::ivec2 pos, glm::ivec2 wh);
void AddQuad(Quad& quad);
void RemoveObject(Object& object);
void UploadMesh();
void BuildIndexBuffer();
void DrawMesh();

int InitEngine();

bool ShouldClose();

void DoDrawing(Camera& camera);

void CloseEngine();

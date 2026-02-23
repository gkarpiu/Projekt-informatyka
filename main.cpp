#include "engine.h"

constexpr float PLAYER_SPEED=0.1f;

int main(){
    InitEngine();

    Model model;
    Vertex v;
    v.alpha=1;
    v.light=1;

    v.position={0.0f, 0.0f, 0.0f};
    v.uv={0.0f, 0.0f};
    model.vertices.push_back(v);

    v.position={5.0f, 5.0f, 10.0f};
    v.uv={0.1f, 0.1f};
    model.vertices.push_back(v);

    v.position={0.0f, 5.0f, 5.0f};
    v.uv={0.0f, 0.1f};
    model.vertices.push_back(v);

    v.position={-10.0f, -10.0f, -10.0f};
    v.uv={0.0f, 0.0f};
    model.vertices.push_back(v);

    v.position={-5.0f, -5.0f, 0.0f};
    v.uv={0.1f, 0.1f};
    model.vertices.push_back(v);

    v.position={-10.0f, -5.0f, -5.0f};
    v.uv={0.0f, 0.1f};
    model.vertices.push_back(v);

    AddModel(model);

    //Render loop
    while(!ShouldClose()){
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

        DoDrawing(camera);
    }
    CloseEngine();
    return 0;
}
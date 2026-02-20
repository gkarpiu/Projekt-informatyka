#include "engine.h"

constexpr float PLAYER_SPEED=10.0f;

Camera camera;

int main(){
    InitEngine();

    Mesh mesh;
    Quad quad;
    quad.light=1.0f;
    quad.alpha=1.0f;
    quad.sprite=MakeSprite({0,0}, {256,256});

    quad.size={256.0f, 256.0f};
    quad.position={500.0f, 300.0f, 0.0f};
    AddQuad(quad);

    quad.position={1500.0f, 100.0f, 0.0f};
    AddQuad(quad);

    quad.alpha=0.2f;
    quad.size={800.0f, 800.0f};
    quad.position={510.0f, 310.0f, -5.0f};
    AddQuad(quad);

    quad.light=0.5f;
    quad.position={800.0f, 750.0f, 0.0f};
    AddQuad(quad);

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
        if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS) {
            yoffset+=PLAYER_SPEED;
            moved=1;
        }
        if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS) {
            yoffset-=PLAYER_SPEED;
            moved=1;
        }
        if(moved) camera.updatePosition(xoffset, yoffset, zoffset);

        DoDrawing(camera);
    }
    CloseEngine();
    return 0;
}
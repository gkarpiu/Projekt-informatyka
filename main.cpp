#include "Physics.h"

int main(){

    const double targetFrameTime = 1.0f / 60.0f;
    InitEngine();

    //make sure to add texture to "textures" folder
    size_t texDesk=LoadTexture("blat_lawki.png");
    size_t texDoor=LoadTexture("drzwi.png");
    size_t texChair=LoadTexture("lawka siedzenie.png");
    size_t texQuepasa=LoadTexture("pixil-frame-0 (6).png");
    size_t texFloorWood=LoadTexture("podloga drewniana.png");
    size_t texWallConcrete=LoadTexture("sciana betonowa.png");
    size_t texWallConcreteTheSecond=LoadTexture("sciana_betonowa.png");
    size_t texWallPhysics=LoadTexture("sciana_fizyczna+informatyczna.png");
    size_t texWall=LoadTexture("sciana.png");
    size_t texBin=LoadTexture("smietnik.png");
    size_t texBoard=LoadTexture("tablica korkowa.png");

    std::vector<size_t> mesh1, uiThing;
    Node* mesh1bvh=LoadObject("alo.obj", mesh1);
    //Node* uibvh=LoadObject("ui.obj", uiThing);

    size_t e1=AddEntity(mesh1, mesh1bvh, 0, 0);
    //size_t e2=AddEntity(uiThing, uibvh, 0, 1);

    entities[e1].transform=glm::translate(entities[e1].transform, {0, 0, 0});

    camera.Position=spawnPoint;

    //Render loop
    while(!ShouldClose())
    {
        double startTime = glfwGetTime();
        DoMovement(camera, window);
        DoDrawing(camera);
        double frameTime=glfwGetTime()-startTime;
        if (frameTime < targetFrameTime)
            std::this_thread::sleep_for(std::chrono::duration<double>(targetFrameTime-frameTime));
        if(glfwGetKey(window, GLFW_KEY_R)) {camera.Position=spawnPoint; playerVelocity=glm::vec3{0.0f};}
        std::cout<<frameTime/targetFrameTime<<"x target\n";
    }
    CloseEngine();
    return 0;
}
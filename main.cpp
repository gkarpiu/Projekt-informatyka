#include "Physics.h"

int main(){

    const double targetFrameTime = 1.0 / 60.0;
    InitEngine();

    //make sure to add texture to "textures" folder
    size_t texConcrete=LoadTexture("concrete.png");
    size_t texWood=LoadTexture("wood.png");
    size_t texPlastic=LoadTexture("plastic.png");
    size_t texMetal=LoadTexture("metal.png");
    size_t texBlack=LoadTexture("black.png");
    size_t texNext=LoadTexture("next.png");

    std::vector<size_t> mesh1, uiThing;
    Node* mesh1bvh=LoadObject("ALO (no walce).obj", mesh1);
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
        {
            glfwWaitEventsTimeout(targetFrameTime-frameTime);
            std::cout<<"fine\n";
        }
        else
            std::cout<<frameTime/targetFrameTime<<"x target\n";
    }
    CloseEngine();
    return 0;
}
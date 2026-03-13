#include "Physics.h"

int main(){
    InitEngine();

    //make sure to add texture to "textures" folder
    size_t texConcrete=LoadTexture("concrete.png");
    size_t texWood=LoadTexture("wood.png");
    size_t texPlastic=LoadTexture("plastic.png");

    size_t mesh1=LoadObject("thatthing.obj");

    size_t e1=AddEntity(mesh1, mesh1, texConcrete, 0);
    size_t e2=AddEntity(mesh1, mesh1, texWood, 0);

    entities[e1].transform=glm::translate(entities[e1].transform, {10, 10, 10});

    camera.Position=spawnPoint;

    //Render loop
    while(!ShouldClose())
    {
        //entities[e1].transform=glm::rotate(entities[e1].transform, 0.01f, {0, 1, 0});
        //entities[e2].transform=glm::rotate(entities[e2].transform, 0.01f, {-1, 0, 0});

        DoMovement(camera, window);
        std::vector<size_t> triggerIds;
        CheckTriggers(camera, triggerIds);
        for(size_t id : triggerIds) std::cout<<"entity: "<<id<<" collided\n";

        DoDrawing(camera);
    }
    CloseEngine();
    return 0;
}
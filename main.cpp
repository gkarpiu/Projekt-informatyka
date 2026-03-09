#include "Physics.h"

int main(){
    InitEngine();

    size_t mesh1=LoadObject("thatthing.obj");

    size_t e1=AddEntity(mesh1, mesh1, 0);
    size_t e2=AddEntity(mesh1, mesh1, 1);

    entities[e1].transform=glm::translate(entities[e1].transform, {10, 10, 10});

    //Render loop
    while(!ShouldClose())
    {
        DoMovement(camera, window);

        std::vector<size_t> triggerIds;
        CheckTriggers(camera, triggerIds);
        for(size_t id : triggerIds) std::cout<<"entity: "<<id<<" collided\n";

        DoDrawing(camera);
    }
    CloseEngine();
    return 0;
}
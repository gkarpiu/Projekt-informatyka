#include "engine.h"
#include "Physics.h"

int main(){
    InitEngine();

    int mesh1=LoadObject("test.obj");

    size_t e1=AddEntity(mesh1);
    size_t e2=AddEntity(mesh1);

    entities[e1].transform=glm::translate(entities[e1].transform, {10, 10, 10});

    //Render loop
    while(!ShouldClose()){
        DoMovement(camera, window);

        entities[e1].transform=glm::rotate(entities[e1].transform, 0.01f, {1.0f, 1.0f, 1.0f});
        entities[e2].transform=glm::rotate(entities[e2].transform, -0.01f, {1.0f, 1.0f, 1.0f});

        DoDrawing(camera);
    }
    CloseEngine();
    return 0;
}
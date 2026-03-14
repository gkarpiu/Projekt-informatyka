#include "Physics.h"

int main(){
    InitEngine();

    //make sure to add texture to "textures" folder
    size_t texConcrete=LoadTexture("concrete.png");
    size_t texWood=LoadTexture("wood.png");
    size_t texPlastic=LoadTexture("plastic.png");
    size_t texMetal=LoadTexture("metal.png");
    size_t texBlack=LoadTexture("black.png");
    size_t texNext=LoadTexture("next.png");

    std::vector<size_t> mesh1, uiThing;
    size_t hitbox1=LoadObject("thatthing.obj", mesh1);
    LoadObject("ui.obj", uiThing);

    size_t e1=AddEntity(mesh1, hitbox1, 0, 0);
    size_t e2=AddEntity(uiThing, 0, 0, 1);

    entities[e1].transform=glm::translate(entities[e1].transform, {-50, -50, -50});

    camera.Position=spawnPoint;

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
#include "engine.h"
#include "Physics.h"

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
        
        DoMovement(camera, window);
        
        DoDrawing(camera);
    }
    CloseEngine();
    return 0;
}
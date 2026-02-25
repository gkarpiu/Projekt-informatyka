#include "engine.h"
#include "Physics.h"

int main(){
    InitEngine();

    Mesh mesh;
    Vertex v;
    v.alpha=1;
    v.light=1;

    v.position={0.0f, 0.0f, 0.0f};
    v.uv={0.0f, 0.0f};
    mesh.vertices.push_back(v);

    v.position={5.0f, 5.0f, 10.0f};
    v.uv={0.1f, 0.1f};
    mesh.vertices.push_back(v);

    v.position={0.0f, 5.0f, 5.0f};
    v.uv={0.0f, 0.1f};
    mesh.vertices.push_back(v);

    mesh.indices.push_back(0);
    mesh.indices.push_back(1);
    mesh.indices.push_back(2);

    meshes.emplace_back();
    UploadMesh(mesh, meshes.back());

    AddEntity(0);
    AddEntity(0);
    AddEntity(0);

    entities[1].transform=glm::translate(entities[1].transform, {10, 0, 0});
    entities[2].transform=glm::translate(entities[1].transform, {-5, 10, 5});

    //Render loop
    while(!ShouldClose()){
        DoMovement(camera, window);

        entities[0].transform=glm::rotate(entities[0].transform, glm::radians(0.2f), {0, 0, 1});
        
        DoDrawing(camera);
    }
    CloseEngine();
    return 0;
}
#include "Physics.h"

const float playerAcceleration=0.1f;
const float jumpStrength=0.3f;
const glm::vec3 gravity={0.0f, -0.01f, 0.0f};
glm::vec3 playerVelocity={0.0f, 0.0f, 0.0f};
const AABB playerHitbox={{0.0f, -1.0f, 0.0f}, {0.25f, 1.4f, 0.25f}};
//const glm::vec3 spawnPoint={33.4f, -7.0f, 73.0f};
const glm::vec3 spawnPoint={0.0f, 0.0f, 0.0f};
std::vector<Triangle> lastCollisions={};

bool TakeInput(GLFWwindow* window, glm::vec3& offset)
{
    bool moved=0;
    if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS) {
        offset.x+=playerAcceleration;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS) {
        offset.x-=playerAcceleration;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS) {
        offset.z-=playerAcceleration;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS) {
        offset.z+=playerAcceleration;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS) {
        for(Triangle triangle : lastCollisions){

            if(triangle.normal.y<0.0f) continue; //triangle is downward facing
            if(TestIntersect(triangle, playerHitbox, camera.Position+playerVelocity+gravity)){ //on ground
                offset.y+=jumpStrength;
                return 1; //break loop and return moved
            }
        }
    }
    return moved;
}

//https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/aabb-triangle.html
bool TestIntersect(Triangle triangle, const AABB& aabb, glm::vec3 playerPos)
{
    //centering triangle
    triangle.p1-=aabb.position+playerPos;
    triangle.p2-=aabb.position+playerPos;
    triangle.p3-=aabb.position+playerPos;
    //triangle edges
    glm::vec3 ev1=triangle.p2-triangle.p1;
    glm::vec3 ev2=triangle.p3-triangle.p2;
    glm::vec3 ev3=triangle.p1-triangle.p3;
    //aabb normals
    glm::vec3 nv1={1.0f, 0.0f, 0.0f};
    glm::vec3 nv2={0.0f, 1.0f, 0.0f};
    glm::vec3 nv3={0.0f, 0.0f, 1.0f};

    //9 cross product axes
    std::vector<glm::vec3> axes;
    axes.emplace_back(glm::cross(ev1, nv1));
    axes.emplace_back(glm::cross(ev1, nv2));
    axes.emplace_back(glm::cross(ev1, nv3));

    axes.emplace_back(glm::cross(ev2, nv1));
    axes.emplace_back(glm::cross(ev2, nv2));
    axes.emplace_back(glm::cross(ev2, nv3));

    axes.emplace_back(glm::cross(ev3, nv1));
    axes.emplace_back(glm::cross(ev3, nv2));
    axes.emplace_back(glm::cross(ev3, nv3));
    //aabb normals
    axes.emplace_back(nv1);
    axes.emplace_back(nv2);
    axes.emplace_back(nv3);
    //triangle normal  
    axes.emplace_back(glm::cross(ev1, ev2));

    for(glm::vec3 axis: axes)
    {
        if(glm::length(axis)<1e-8f) continue;
        if(!testSAT(triangle, aabb, axis)) return 0;
    }
    return 1;
}

bool testSAT(Triangle triangle, const AABB& aabb, glm::vec3 axis)
{
    float proj1=glm::dot(triangle.p1, axis);
    float proj2=glm::dot(triangle.p2, axis);
    float proj3=glm::dot(triangle.p3, axis);
    float r=aabb.extents.x*std::abs(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), axis))+
            aabb.extents.y*std::abs(glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), axis))+
            aabb.extents.z*std::abs(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), axis));
    float maxProj=std::max(std::max(proj1, proj2), proj3);
    float minProj=std::min(std::min(proj1, proj2), proj3);
    return !(maxProj<-r || minProj>r);
}

bool CheckCollision(Entity& entity, Camera& camera){
    for(Triangle triangle : collisions[entity.hitbox]){
        triangle.p1=glm::vec3(entity.transform*glm::vec4(triangle.p1, 1.0f));
        triangle.p2=glm::vec3(entity.transform*glm::vec4(triangle.p2, 1.0f));
        triangle.p3=glm::vec3(entity.transform*glm::vec4(triangle.p3, 1.0f));
        if(TestIntersect(triangle, {playerHitbox.position, playerHitbox.extents}, camera.Position)) return 1;
    }
    return 0;
}

void ResolveCollision(Entity& entity, glm::vec3& velocity, Camera& camera){
    lastCollisions.clear();
    for(Triangle triangle : collisions[entity.hitbox]){

        if(glm::dot(velocity, triangle.normal)>=0.0f) continue; //dont check if moving away from triangle

        triangle.p1=glm::vec3(entity.transform*glm::vec4(triangle.p1, 1.0f));
        triangle.p2=glm::vec3(entity.transform*glm::vec4(triangle.p2, 1.0f));
        triangle.p3=glm::vec3(entity.transform*glm::vec4(triangle.p3, 1.0f));
        if(TestIntersect(triangle, playerHitbox, camera.Position+velocity)){
            lastCollisions.push_back(triangle);
            velocity=velocity-glm::dot(velocity, triangle.normal)*triangle.normal;
        }
    }
}

void CheckTriggers(Camera& camera, std::vector<size_t>& ids){
    for(size_t i=0; i<entities.size(); i++){
        if(!entities[i].trigger) continue;
        if(CheckCollision(entities[i], camera)) ids.push_back(i);
    }
}

void DoMovement(Camera& camera, GLFWwindow* window){
    glm::vec3 offset={0.0f, 0.0f, 0.0f };
    TakeInput(window, offset);
    glm::vec3 camOffset=camera.ToCamVector(offset);
    playerVelocity+=gravity;
    playerVelocity.x=(camOffset.x+playerVelocity.x)*0.5f;
    playerVelocity.y+=camOffset.y;
    playerVelocity.z=(camOffset.z+playerVelocity.z)*0.5f;
    for(Entity& e: entities){
        if(e.trigger) continue;
        ResolveCollision(e, playerVelocity, camera);
    }
    camera.updatePosition(playerVelocity);
}
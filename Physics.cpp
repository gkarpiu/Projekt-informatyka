#include "Physics.h"

const float PLAYER_SPEED=0.1f;
const AABB playerHitbox={{0.0f, -0.6f, 0.0f}, {0.25f, 0.9f, 0.25f}};

bool TakeInput(GLFWwindow* window, float& x, float& y, float& z)
{
    bool moved=0;
    if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS) {
        x+=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS) {
        x-=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS) {
        y-=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS) {
        y+=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS) {
        z-=PLAYER_SPEED;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS) {
        z+=PLAYER_SPEED;
        moved=1;
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

void DoMovement(Camera& camera, GLFWwindow* window)
{
    float xoffset=0, yoffset=0, zoffset=0;
    if(TakeInput(window, xoffset, yoffset, zoffset)){
        glm::vec3 velocity=camera.ToCamVector(xoffset, yoffset, zoffset);
        for(Entity e: entities){
            for(Triangle triangle : collisions[e.hitbox]){
                triangle.p1+=glm::vec3(e.transform[3]);
                triangle.p2+=glm::vec3(e.transform[3]);
                triangle.p3+=glm::vec3(e.transform[3]);
                if(TestIntersect(triangle, {playerHitbox.position, playerHitbox.extents}, camera.Position+velocity)){
                    velocity=velocity-glm::dot(velocity, triangle.normal)*triangle.normal;
                }
            }
        }
        camera.updatePosition(velocity);
    }
}
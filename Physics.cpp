#include "Physics.h"

const float playerAcceleration=0.2f;
const float playerSprintAcceleration=playerAcceleration*2.0f;
const float playerCrouchAcceleration=playerAcceleration*0.5f;
const float jumpStrength=0.4f;

const float slideStrength=0.7f;
bool isCrouching=0;
float crouchTimer=1.0f;

const glm::vec3 gravity={0.0f, -0.02f, 0.0f};
glm::vec3 playerVelocity={0.0f, 0.0f, 0.0f};

const AABB playerDefaultHitbox={{0.0f, -1.0f, 0.0f}, {0.25f, 1.4f, 0.25f}};
const AABB playerCrouchHitbox={{0.0f, -0.5f, 0.0f}, {0.25f, 0.7f, 0.25f}};
const float hitboxDiff=playerDefaultHitbox.extents.y-playerCrouchHitbox.extents.y-(playerDefaultHitbox.position.y-playerCrouchHitbox.position.y);
AABB playerHitbox=playerDefaultHitbox;

const glm::vec3 spawnPoint={0.0f, 0.0f, 0.0f};
std::vector<Triangle> lastCollisions={};
bool isOnGround=0;

bool CollisionAABB(const AABB& a, const AABB& b){
    return (a.position.x-a.extents.x<=b.position.x+b.extents.x && a.position.x+a.extents.x>=b.position.x-b.extents.x) &&
           (a.position.y-a.extents.y<=b.position.y+b.extents.y && a.position.y+a.extents.y>=b.position.y-b.extents.y) &&
           (a.position.z-a.extents.z<=b.position.z+b.extents.z && a.position.z+a.extents.z>=b.position.z-b.extents.z);
}

bool TakeInput(GLFWwindow* window, glm::vec3& offset)
{
    bool moved=0;
    bool crouchFirstPress=0;
    float tmp=playerAcceleration;

    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)==GLFW_PRESS)
    {
        tmp=playerCrouchAcceleration;
        if(!isCrouching)
        {
            camera.Position.y-=hitboxDiff; //crouch
            crouchFirstPress=1;
        }
        isCrouching=1;
    }else{
        if(isCrouching) camera.Position.y+=hitboxDiff; //uncrouch
        isCrouching=0;
    }
    
    
    if (glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS) {
        offset.x+=tmp;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS) {
        offset.x-=tmp;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS) {
        offset.z-=tmp;
        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS) {
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS)
        {
            if(crouchFirstPress&&isOnGround) crouchTimer=0.0f;
            if(!isCrouching) tmp=playerSprintAcceleration;
        }
        if(crouchTimer<1.0f&&(isCrouching||!isOnGround))
        {
            crouchTimer+=0.0166f;
            offset.z+=(1.0f-crouchTimer)*slideStrength;
        }else offset.z+=tmp;

        moved=1;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS) {
        if(isOnGround){
            offset.y+=jumpStrength;
            return 1;
        }
    }
    return moved;
}

//https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/aabb-triangle.html
bool TestIntersect(Triangle triangle, const AABB& aabb)
{
    //centering triangle
    triangle.p1-=aabb.position;
    triangle.p2-=aabb.position;
    triangle.p3-=aabb.position;
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

void CheckCollision(const AABB localPlayer, const Node* node, glm::vec3& velocity){
    if(!CollisionAABB(localPlayer, node->aabb)) return;
    if(node->isLeaf)
    {
        for(int i=node->start;i<node->start+node->count;i++)
        {
            if(glm::dot(collisions[node->collisionId][i].normal, playerVelocity)>=0.0f) continue;
            if(TestIntersect(collisions[node->collisionId][i], localPlayer))
            {
                if(collisions[node->collisionId][i].normal.y>0.0f)
                {
                    isOnGround=1;
                }
                if(!std::isfinite(collisions[node->collisionId][i].normal.x) || !std::isfinite(collisions[node->collisionId][i].normal.y) || !std::isfinite(collisions[node->collisionId][i].normal.z)) continue;
                velocity-=collisions[node->collisionId][i].normal*glm::dot(velocity, collisions[node->collisionId][i].normal);
                //if(glm::isnan(velocity.x) || glm::isnan(velocity.y) || glm::isnan(velocity.z)) velocity=glm::vec3(0.0f);
            }
        }
    }else{
        CheckCollision(localPlayer, node->l, velocity);
        CheckCollision(localPlayer, node->r, velocity);
    }
}

void DoMovement(Camera& camera, GLFWwindow* window){
    glm::vec3 offset={0.0f, 0.0f, 0.0f};
    TakeInput(window, offset);
    if(isCrouching) playerHitbox=playerCrouchHitbox;
    else playerHitbox=playerDefaultHitbox;

    glm::vec3 camOffset=camera.ToCamVector(offset);
    playerVelocity+=gravity;
    playerVelocity.x=camOffset.x;
    playerVelocity.y+=camOffset.y;
    playerVelocity.z=camOffset.z;
    //std::cout<<camera.Position.x<<" "<<camera.Position.y<<" "<<camera.Position.z<<"\n";

    isOnGround=0;
    for(Entity& e: entities){
        if(e.trigger) continue;
        if(glm::abs(glm::determinant(e.transform))<1e-8f) continue;
        AABB localPlayer=playerHitbox;
        localPlayer.position=glm::vec3(glm::inverse(e.transform)*glm::vec4(playerHitbox.position+camera.Position, 1.0f));
        CheckCollision(localPlayer, e.bvh, playerVelocity);
    }
    camera.updatePosition(playerVelocity);
}
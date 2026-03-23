#define STB_IMAGE_IMPLEMENTATION
#include <image/stb_image.h>
#include "engine.h"

const char* worldvertexShaderSource=
"#version 460 core\n"
"layout (location=0) in vec3 aPos;\n"
"layout (location=1) in vec2 aUV;\n"
"layout (location=2) in float aLight;\n"
"layout (location=3) in float aAlpha;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out float vLight;\n"
"out vec2 vUV;\n"
"out float vAlpha;\n"
"void main()\n"
"{\n"
"    vLight = aLight;\n"
"    vUV = aUV;\n"
"    vAlpha = aAlpha;\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

const char* worldfragmentShaderSource=
"#version 460 core\n"
"out vec4 FragColor;\n"
"in vec2 vUV;\n"
"in float vLight;\n"
"in float vAlpha;\n"
"uniform sampler2D tex;\n"
"void main()\n"
"{\n"
"    vec4 t = texture(tex, vUV);\n"
"    FragColor = vec4(t.rgb*vLight, t.a*vAlpha);\n"
"}\n";

const char* uivertexShaderSource=
"#version 460 core\n"
"layout (location=0) in vec3 aPos;\n"
"layout (location=1) in vec2 aUV;\n"
"layout (location=2) in float aLight;\n"
"layout (location=3) in float aAlpha;\n"
"uniform mat4 projection;\n"
"out float vLight;\n"
"out vec2 vUV;\n"
"out float vAlpha;\n"
"void main()\n"
"{\n"
"    vLight = aLight;\n"
"    vUV = aUV;\n"
"    vAlpha = aAlpha;\n"
"    gl_Position = projection * vec4(aPos, 1.0);\n"
"}\n";

const char* uifragmentShaderSource=
"#version 460 core\n"
"out vec4 FragColor;\n"
"in vec2 vUV;\n"
"in float vLight;\n"
"in float vAlpha;\n"
"uniform sampler2D tex;\n"
"void main()\n"
"{\n"
"    vec4 t = texture(tex, vUV);\n"
"    FragColor = vec4(t.rgb*vLight, t.a*vAlpha);\n"
"}\n";

int WINDOW_SCALE=1;
GLFWwindow* window=nullptr;
unsigned int modelLoc;
unsigned int viewLoc;
unsigned int worldprojLoc;
unsigned int uiprojLoc;
unsigned int worldshaderProgram;
unsigned int uishaderProgram;
bool firstMouse=1;
float lastX, lastY;
const uint8_t LEAF_SIZE=16;
std::vector<Renderer> meshes(1);
std::vector<Entity> entities;
std::vector<Entity> uiThings;
std::vector<Texture> textures;
std::vector<std::vector<Triangle>> collisions(1);
glm::mat4 worldprojection=glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 1000.0f);
glm::mat4 uiprojection=glm::ortho(0, 1920, 0, 1080);

Camera camera;

Triangle::Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3){
    this->p1=p1;
    this->p2=p2;
    this->p3=p3;

    glm::vec3 t1=p2-p1;
    glm::vec3 t2=p3-p1;
    normal=glm::normalize(glm::cross(t1,t2));
}

size_t LoadTexture(const char* path){
    textures.emplace_back();
    int channels;

    stbi_set_flip_vertically_on_load(true);

    std::cout<<"Loading texture "<<path<<"\n";
    std::vector<std::string> paths={"./textures/", "./textures/uiStuff/", ""};
    unsigned char* data;
    for(std::string p : paths){
        data=stbi_load((p + path).c_str(), &textures.back().width, &textures.back().height, &channels, 0);
        if(data){
            std::cout<<"Loaded from "<<(p + path).c_str()<<"\n";
            break;
        }
    }
    if(!data){
        std::cout<<"Failed to load texture "<<path<<"\n";
        return -1;
    }

    GLenum format=GL_RGB;
    if(channels==4)
        format=GL_RGBA;

    glGenTextures(1, &textures.back().id);
    glBindTexture(GL_TEXTURE_2D, textures.back().id);

    glTexImage2D(GL_TEXTURE_2D, 0, format, textures.back().width, textures.back().height, 0, format,  GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
    
    std::cout<<"Texture size: "<<textures.back().width<<"x"<<textures.back().height<<" Channels: "<<channels<<"\n\n";
    return textures.size()-1;
}

const glm::vec3 Centroid(const Triangle& a){
    return (a.p1+a.p2+a.p3)/3.0f;
}

AABB computeBounds(const std::vector<Triangle>& prims, int start, int end){
    glm::vec3 min=prims[start].p1, max=prims[start].p1;
    for(int i=start; i<end; i++){
        Triangle t=prims[i];
        if(t.p1.x<min.x) min.x=t.p1.x;
        else if(t.p1.x>max.x) max.x=t.p1.x;
        if(t.p1.y<min.y) min.y=t.p1.y;
        else if(t.p1.y>max.y) max.y=t.p1.y;
        if(t.p1.z<min.z) min.z=t.p1.z;
        else if(t.p1.z>max.z) max.z=t.p1.z;

        if(t.p2.x<min.x) min.x=t.p2.x;
        else if(t.p2.x>max.x) max.x=t.p2.x;
        if(t.p2.y<min.y) min.y=t.p2.y;
        else if(t.p2.y>max.y) max.y=t.p2.y;
        if(t.p2.z<min.z) min.z=t.p2.z;
        else if(t.p2.z>max.z) max.z=t.p2.z;

        if(t.p3.x<min.x) min.x=t.p3.x;
        else if(t.p3.x>max.x) max.x=t.p3.x;
        if(t.p3.y<min.y) min.y=t.p3.y;
        else if(t.p3.y>max.y) max.y=t.p3.y;
        if(t.p3.z<min.z) min.z=t.p3.z;
        else if(t.p3.z>max.z) max.z=t.p3.z;
    }
    return {(min+max)/2.0f, glm::abs((max-min)/2.0f)};
}

Node* CreateBVH(std::vector<Triangle>& prims, int start, int end, size_t collisionId)
{
    Node* node=new Node();
    node->aabb=computeBounds(prims, start, end);

    node->start=start;
    node->count=end-start;
    node->collisionId=collisionId;
    if(node->count<=LEAF_SIZE){
        node->l=node->r=nullptr;
        node->isLeaf=1;
        return node;
    }

    int axis;
    if(node->aabb.extents.x>node->aabb.extents.y){
        if(node->aabb.extents.x>node->aabb.extents.z) axis=0;
        else axis=2;
    }else{
        if(node->aabb.extents.y>node->aabb.extents.z) axis=1;
        else axis=2;
    }

    int mid=(start+end)/2;
    std::nth_element(prims.begin()+start, prims.begin()+mid, prims.begin()+end, [axis](const Triangle& a, const Triangle& b){return Centroid(a)[axis]<Centroid(b)[axis];});

    node->l=CreateBVH(prims, start, mid, collisionId);
    node->r=CreateBVH(prims, mid, end, collisionId);
    node->isLeaf=0;

    return node;
}
Node* LoadObject(std::string name, std::vector<size_t>& meshVec){
    std::cout<<"Loading object "<<name<<"\n";
    Mesh mesh;
    collisions.emplace_back();
    std::vector<glm::vec3> tpos;
    std::vector<glm::vec3> tnor;
    std::vector<glm::vec2> tuv;

    std::ifstream inf(name);
    
    std::string line;
    while(std::getline(inf, line)){
        std::istringstream ss(line);
        std::string type;
        ss>>type;
        
        if(type=="usemtl"){
            mesh.parts.emplace_back();
            ss>>mesh.parts.back().texture;
        }else if(type=="v"){
            tpos.emplace_back();
            ss>>tpos.back().x>>tpos.back().y>>tpos.back().z;
        }else if(type=="vn"){
            tnor.emplace_back();
            ss>>tnor.back().x>>tnor.back().y>>tnor.back().z;
        }else if(type=="vt"){
            tuv.emplace_back();
            ss>>tuv.back().x>>tuv.back().y;
        }else if(type=="f"){
            std::string vd;
            std::vector<glm::vec3> points;
            points.clear();
            while(ss>>vd){
                std::istringstream vs(vd);
                std::string ts;
                std::getline(vs, ts, '/');
                int posId=std::stoi(ts)-1;
                std::getline(vs, ts, '/');
                int uvId=std::stoi(ts)-1;
                std::getline(vs, ts);
                int norId=std::stoi(ts)-1;
                float t=1-((tnor[norId].y+1)/2);
                mesh.parts.back().vertices.push_back({tpos[posId], tnor[norId], tuv[uvId], 0.4f*t+1.0f*(1-t), 1.0f});
                mesh.parts.back().indices.emplace_back(mesh.parts.back().vertices.size()-1);
                points.push_back(tpos[posId]);
            }
            collisions.back().push_back({points[0], points[1], points[2]});
        }
    }
    inf.close();
    std::cout<<"parsing complete\n";

    for(MeshPart m : mesh.parts){
        meshes.emplace_back();
        UploadMesh(m, meshes.back());
        meshVec.push_back(meshes.size()-1);
    }

    std::cout<<"creating bvh..........\n";
    Node* bvh=CreateBVH(collisions.back(), 0, collisions.back().size(), collisions.size()-1);
    std::cout<<"bvh complete\n";

    return bvh;
}
void UploadMesh(MeshPart& mesh, Renderer& renderer){
    renderer.texture=mesh.texture;
    renderer.idCount=mesh.indices.size();
    glGenVertexArrays(1, &renderer.VAO);
    glGenBuffers(1, &renderer.VBO);
    glGenBuffers(1, &renderer.EBO);

    glBindVertexArray(renderer.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, renderer.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size()*sizeof(Vertex), mesh.vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, light));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, alpha));

    glBindVertexArray(0);
}

size_t AddEntity(std::vector<size_t>& mesh, Node* hitbox, bool trigger, bool ui){
    size_t id;
    if(ui){
        uiThings.push_back({glm::mat4(1.0f), mesh, hitbox, trigger});
        id=uiThings.size()-1;
    }else{
        entities.push_back({glm::mat4(1.0f), mesh, hitbox, trigger});
        id=entities.size()-1;
    }
    return id;
}

void DrawEntity(Entity& entity){
    for(int i=0; i<entity.mesh.size(); i++){
        glBindTexture(GL_TEXTURE_2D, textures[meshes[entity.mesh[i]].texture].id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindVertexArray(meshes[entity.mesh[i]].VAO);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(entity.transform));
        glDrawElements(GL_TRIANGLES, meshes[entity.mesh[i]].idCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX=xpos;
        lastY=ypos;
        firstMouse=0;
    }

    float xoffset=xpos-lastX;
    float yoffset=lastY-ypos;

    lastX=xpos;
    lastY=ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

int InitEngine(){
    if(!glfwInit()){
        std::cout<<"GLFW init failed\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor=glfwGetPrimaryMonitor();
    const GLFWvidmode* mode=glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    //WINDOW_SCALE=mode->width/WINDOW_WIDTH;

    //std::cout<<"window: "<<mode->width<<"x"<<mode->height<<" scale: "<<WINDOW_SCALE<<"\n";

    window=glfwCreateWindow(WINDOW_SCALE*WINDOW_WIDTH,WINDOW_SCALE*WINDOW_HEIGHT, "game", nullptr, nullptr);
    if(!window){
        std::cout<<"Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGL(glfwGetProcAddress)){
        std::cout<<"Failed to initialize GLAD\n";
        return -1;
    }

    int success;
    char infoLog[512];

    //Compile world vertex shader
    unsigned int worldvertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(worldvertexShader, 1, &worldvertexShaderSource, nullptr);
    glCompileShader(worldvertexShader);

    glGetShaderiv(worldvertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(worldvertexShader, 512, nullptr, infoLog);
        std::cout<<"VERTEX SHADER ERROR:\n"<<infoLog<<std::endl;
    }

    //Compile ui vertex shader
    unsigned int uivertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(uivertexShader, 1, &uivertexShaderSource, nullptr);
    glCompileShader(uivertexShader);

    glGetShaderiv(uivertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(uivertexShader, 512, nullptr, infoLog);
        std::cout<<"VERTEX SHADER ERROR:\n"<<infoLog<<std::endl;
    }

    //Compile world fragment shader
    unsigned int worldfragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(worldfragmentShader, 1, &worldfragmentShaderSource, nullptr);
    glCompileShader(worldfragmentShader);

    glGetShaderiv(worldfragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(worldfragmentShader, 512, nullptr, infoLog);
        std::cout<<"FRAGMENT SHADER ERROR:\n"<<infoLog<<std::endl;
    }

    //Compile ui fragment shader
    unsigned int uifragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(uifragmentShader, 1, &uifragmentShaderSource, nullptr);
    glCompileShader(uifragmentShader);

    glGetShaderiv(uifragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(uifragmentShader, 512, nullptr, infoLog);
        std::cout<<"FRAGMENT SHADER ERROR:\n"<<infoLog<<std::endl;
    }

    //Link world shaders into a program
    worldshaderProgram=glCreateProgram();
    glAttachShader(worldshaderProgram, worldvertexShader);
    glAttachShader(worldshaderProgram, worldfragmentShader);
    glLinkProgram(worldshaderProgram);

    glGetProgramiv(worldshaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(worldshaderProgram, 512, nullptr, infoLog);
        std::cout<<"PROGRAM LINK ERROR:\n"<<infoLog<<std::endl;
    }

    //Link ui shaders into a program
    uishaderProgram=glCreateProgram();
    glAttachShader(uishaderProgram, uivertexShader);
    glAttachShader(uishaderProgram, uifragmentShader);
    glLinkProgram(uishaderProgram);

    glGetProgramiv(uishaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(uishaderProgram, 512, nullptr, infoLog);
        std::cout<<"PROGRAM LINK ERROR:\n"<<infoLog<<std::endl;
    }

    glDeleteShader(worldvertexShader);
    glDeleteShader(worldfragmentShader);
    glDeleteShader(uivertexShader);
    glDeleteShader(uifragmentShader);

    glUseProgram(worldshaderProgram);
    modelLoc=glGetUniformLocation(worldshaderProgram, "model");
    viewLoc=glGetUniformLocation(worldshaderProgram, "view");
    worldprojLoc=glGetUniformLocation(worldshaderProgram, "projection");
    glUniform1i(glGetUniformLocation(worldshaderProgram, "tex"), 0);
    glUniformMatrix4fv(worldprojLoc, 1, GL_FALSE, glm::value_ptr(worldprojection));

    glUseProgram(uishaderProgram);
    uiprojLoc=glGetUniformLocation(uishaderProgram, "projection");
    glUniform1i(glGetUniformLocation(uishaderProgram, "tex"), 0);
    glUniformMatrix4fv(uiprojLoc, 1, GL_FALSE, glm::value_ptr(uiprojection));


    glViewport(0, 0, WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSwapInterval(0);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    //glClearColor(0.3f, 0.1f, 0.5f, 1.0f); 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                                  //bg color

    return 0;
}

bool ShouldClose(){
    bool res=0;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)) res=1;
    if(glfwWindowShouldClose(window)) res=1;
    return res;
}

void DoDrawing(Camera& camera){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(worldshaderProgram);

    glm::mat4 view=camera.GetViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    glUniformMatrix4fv(worldprojLoc, 1, GL_FALSE, glm::value_ptr(worldprojection));
    for(Entity& e : entities){
        DrawEntity(e);
    }

    glUseProgram(uishaderProgram);

    glUniformMatrix4fv(uiprojLoc, 1, GL_FALSE, glm::value_ptr(uiprojection));
    for(Entity& e : uiThings){
        DrawEntity(e);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void CloseEngine(){
    for(Renderer r : meshes){
        glDeleteVertexArrays(1, &r.VAO);
        glDeleteBuffers(1, &r.VBO);
        glDeleteBuffers(1, &r.EBO);
    }

    glDeleteProgram(worldshaderProgram);
    glfwTerminate();
}


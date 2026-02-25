#define STB_IMAGE_IMPLEMENTATION
#include <image/stb_image.h>

#include "engine.h"

const char* vertexShaderSource=
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

const char* fragmentShaderSource=
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
unsigned int projLoc;
unsigned int shaderProgram;
bool firstMouse;
float lastX, lastY;
std::vector<Renderer> meshes;
std::vector<Entity> entities;

Camera camera;

Texture LoadTexture(const char* path){
    Texture texture;
    int channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data=stbi_load(path, &texture.width, &texture.height, &channels, 0);
    if(!data){
        std::cout<<"Failed to load texture\n";
        return {0,0,0};
    }

    GLenum format=GL_RGB;
    if(channels==4)
        format=GL_RGBA;

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format,  GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
    
    std::cout<<"Texture size: "<<texture.width<<"x"<<texture.height<<" Channels: "<<channels<<"\n";
    return texture;
}

void LoadObject(std::string name){
    Mesh mesh;
    std::vector<glm::vec3> tpos;
    std::vector<glm::vec3> tnor;
    std::vector<glm::vec2> tuv;

    std::ifstream inf(name);
    
    std::string line;
    while(std::getline(inf, line)){
        std::istringstream ss(line);
        std::string type;
        ss>>type;
        
        if(type=="v"){
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
            while(ss>>vd){
                std::istringstream vs(vd);
                std::string ts;
                std::getline(vs, ts, '/');
                int posId=std::stoi(ts)-1;
                std::getline(vs, ts, '/');
                int uvId=std::stoi(ts)-1;
                std::getline(vs, ts);
                int norId=std::stoi(ts)-1;
                mesh.vertices.push_back({tpos[posId], tnor[norId], tuv[uvId], 1.0f, 1.0f});
                mesh.indices.emplace_back(mesh.vertices.size()-1);
            }
        }
    }
    inf.close();
    std::cout<<"Parsed "<<mesh.vertices.size()<<" vertices\n";

    meshes.emplace_back();
    UploadMesh(mesh, meshes.back());
}
void UploadMesh(Mesh& mesh, Renderer& renderer){
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

size_t AddEntity(size_t mesh){
    entities.push_back({mesh, glm::mat4(1.0f)});
    return entities.size()-1;
}

void DrawEntity(Entity& entity){
    glBindVertexArray(meshes[entity.mesh].VAO);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(entity.transform));
    glDrawElements(GL_TRIANGLES, meshes[entity.mesh].idCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed Y

    lastX = xpos;
    lastY = ypos;

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

    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
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

    //Compile vertex shader
    unsigned int vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout<<"VERTEX SHADER ERROR:\n"<<infoLog<<std::endl;
    }

    unsigned int fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout<<"FRAGMENT SHADER ERROR:\n"<<infoLog<<std::endl;
    }

    //Link shaders into a program
    shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout<<"PROGRAM LINK ERROR:\n"<<infoLog<<std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    modelLoc=glGetUniformLocation(shaderProgram, "model");
    viewLoc=glGetUniformLocation(shaderProgram, "view");
    projLoc=glGetUniformLocation(shaderProgram, "projection");

    Texture texture=LoadTexture("atlas.png");
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
    glViewport(0, 0, WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE);
    glm::mat4 projection=glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 1000.0f);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    /*glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);*/
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glClearColor(0.3f, 0.1f, 0.5f, 1.0f);                                   //bg color
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    return 0;
}

bool ShouldClose(){
    return glfwWindowShouldClose(window);
}

void DoDrawing(Camera& camera){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view=camera.GetViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    for(Entity& e : entities){
        DrawEntity(e);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void CloseEngine(){
    for(Entity& e : entities){
        glDeleteVertexArrays(1, &meshes[e.mesh].VAO);
        glDeleteBuffers(1, &meshes[e.mesh].VBO);
        glDeleteBuffers(1, &meshes[e.mesh].EBO);
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
}


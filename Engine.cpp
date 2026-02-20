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
Mesh mesh;
unsigned int modelLoc;
unsigned int viewLoc;
unsigned int projLoc;
glm::ivec2 awh;
unsigned int shaderProgram;

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

    awh={texture.width, texture.height};
    
    std::cout<<"Texture size: "<<texture.width<<"x"<<texture.height<<" Channels: "<<channels<<"\n";
    return texture;
}
void AddObject(Object& object){
    for(int i=0; i<mesh.objects.size(); i++){
        if(object.z<mesh.objects[i].z){
            mesh.objects.insert(mesh.objects.begin()+i, object);
            return;
        }
    }
    mesh.objects.push_back(object);
}
Sprite MakeSprite(glm::ivec2 pos, glm::ivec2 wh){
    Sprite sprite;
    sprite.uv0={pos.x/(float)awh.x, pos.y/(float)awh.y};
    sprite.uv1={(pos.x+wh.x)/(float)awh.x, (pos.y+wh.y)/(float)awh.y};
    return sprite;
}
void AddQuad(Quad& quad){
    glm::vec3 scale(WINDOW_SCALE);
    uint32_t id;
    if(mesh.free.size()>0){
        id=mesh.free.back();
        mesh.vertices[id]={quad.position*scale, quad.sprite.uv0, quad.light, quad.alpha};
        mesh.vertices[id+1]={(quad.position+glm::vec3(quad.size.x, 0.0f, 0.0f))*scale, {quad.sprite.uv1.x, quad.sprite.uv0.y}, quad.light, quad.alpha};
        mesh.vertices[id+2]={(quad.position+glm::vec3(quad.size, 0.0f))*scale, quad.sprite.uv1, quad.light, quad.alpha};
        mesh.vertices[id+3]={(quad.position+glm::vec3(0.0f, quad.size.y, 0.0f))*scale, {quad.sprite.uv0.x, quad.sprite.uv1.y}, quad.light, quad.alpha};
        mesh.free.pop_back();
    }else{
        id=mesh.vertices.size();
        mesh.vertices.push_back({quad.position*scale, quad.sprite.uv0, quad.light, quad.alpha});
        mesh.vertices.push_back({(quad.position+glm::vec3(quad.size.x, 0.0f, 0.0f))*scale, {quad.sprite.uv1.x, quad.sprite.uv0.y}, quad.light, quad.alpha});
        mesh.vertices.push_back({(quad.position+glm::vec3(quad.size, 0.0f))*scale, quad.sprite.uv1, quad.light, quad.alpha});
        mesh.vertices.push_back({(quad.position+glm::vec3(0.0f, quad.size.y, 0.0f))*scale, {quad.sprite.uv0.x, quad.sprite.uv1.y}, quad.light, quad.alpha});
        mesh.indexoffset+=4;
    }

    Object object;
    object.indexoffset=id;
    object.z=quad.position.z;
    object.indices.push_back(id);
    object.indices.push_back(id+1);
    object.indices.push_back(id+2);
    object.indices.push_back(id+0);
    object.indices.push_back(id+3);
    object.indices.push_back(id+2);

    mesh.dirty=1;

    AddObject(object);
}
void RemoveObject(Object& object){
    object.alive=0;
    mesh.dirty=1;
    mesh.free.push_back(object.indexoffset);
}
void UploadMesh(){
    glGenVertexArrays(1, &mesh.renderer.VAO);
    glGenBuffers(1, &mesh.renderer.VBO);
    glGenBuffers(1, &mesh.renderer.EBO);

    glBindVertexArray(mesh.renderer.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.renderer.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size()*sizeof(Vertex), mesh.vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.renderer.EBO);
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
void BuildIndexBuffer(){
    mesh.indices.clear();
    for(Object& o : mesh.objects){
        if(o.alive) mesh.indices.insert(mesh.indices.end(), o.indices.begin(), o.indices.end());
    }
}
void DrawMesh(){
    glBindVertexArray(mesh.renderer.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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

    WINDOW_SCALE=mode->width/WINDOW_WIDTH;

    std::cout<<"window: "<<mode->width<<"x"<<mode->height<<" scale: "<<WINDOW_SCALE<<"\n";

    window=glfwCreateWindow(mode->width, mode->height, "yes", nullptr, nullptr);
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
    glm::mat4 projection=glm::ortho(0.0f, (float)WINDOW_WIDTH*WINDOW_SCALE, 0.0f, (float)WINDOW_HEIGHT*WINDOW_SCALE, -100.0f, 100.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);                                   //bg color
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
    glm::mat4 model=glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    if(mesh.dirty){
        BuildIndexBuffer();
        UploadMesh();
    }

    DrawMesh();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void CloseEngine(){
    glDeleteVertexArrays(1, &mesh.renderer.VAO);
    glDeleteBuffers(1, &mesh.renderer.VBO);
    glDeleteBuffers(1, &mesh.renderer.EBO);

    glDeleteProgram(shaderProgram);
    glfwTerminate();
}


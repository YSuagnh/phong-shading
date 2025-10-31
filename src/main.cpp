#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "shader.h"
#include "loadobj.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char** argv) {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Test", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    glEnable(GL_DEPTH_TEST);    

    Shader phong_shader(std::string(SHADER_DIR) + "/phong-vertex.vs",
                  std::string(SHADER_DIR) + "/phong-fragment.fs");
    
    Shader gouraud_shader(std::string(SHADER_DIR) + "/gouraud-vertex.vs",
                  std::string(SHADER_DIR) + "/gouraud-fragment.fs");

    // Cook-Torrance (PBR) shader
    Shader cook_shader(std::string(SHADER_DIR) + "/cooktorrance-vertex.vs",
                  std::string(SHADER_DIR) + "/cooktorrance-fragment.fs");
    
    int vertexCount = 0;
    std::string objName = "cube";
    if(argc > 1) objName = argv[1];
    std::string objPath = std::string(TEST_DIR) + "/" + objName + ".obj";
    std::vector<float> vertices = loadOBJ(objPath, vertexCount);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // 设置顶点属性指针: layout(0)=pos, layout(1)=normal
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);    

    // render loop
    // -----------

    glm::vec3 lightPos(4.0f, 4.0f, 4.0f);
    glm::vec3 viewPos(0.0f, 0.0f, 4.0f);
    glm::vec3 lightcolor(1.0f, 1.0f, 1.0f);
    glm::vec3 objcolor(0.0f, 0.5f, 1.5f);
    glm::vec3 F0(0.15f, 0.15f, 0.15f);
    glm::vec3 albedo(0.15f, 0.15f, 0.15f);
    glm::float32 ambient(0.2f);
    glm::float32 specular(0.5f);
    glm::float32 shininess(60.0f);
    glm::float32 metallic(1.0f);
    // parameters used by Cook-Torrance shader
    float roughness = 0.35f;
    glm::mat4 rot = glm::mat4(1.0f);
    auto shader = &phong_shader;
    
    if(argc > 2) {
        if(argv[2][0] == '1') {
            shader = &gouraud_shader;       // Gouraud
        } else if(argv[2][0] == '2') {
            shader = &cook_shader;      // Cook-Torrance
            ambient = 0.1f;
            lightcolor = glm::vec3(3.0f, 3.0f, 3.0f);
        }
        if(argc > 3) {
            if(argv[3][0] == '1') { //铜
                objcolor = F0 = glm::vec3(0.955f, 0.638f, 0.538f);
                metallic = 1.0f;
                albedo = glm::vec3(1.0f, 0.8f, 0.6f);
                roughness = 0.2f;
            } else if(argv[3][0] == '2') { //银
                objcolor = F0 = glm::vec3(0.660f, 0.670f, 0.680f);
                metallic = 1.0f;
                albedo = glm::vec3(0.9f, 0.9f, 0.9f);
                roughness = 0.4;
            } else if(argv[3][0] == '3') { //皮革
                objcolor = albedo = glm::vec3(0.23f, 0.12f, 0.05f);
                F0 = glm::vec3(0.04f, 0.04f, 0.04f);
                metallic = 0.0f;
                roughness = 0.5f;
            } else if(argv[3][0] == '4') { //铁
                objcolor = F0 = glm::vec3(0.560f, 0.570f, 0.580f);
                metallic = 1.0f;
                albedo = glm::vec3(0.65f, 0.65f, 0.65f);
                roughness = 0.3f;
            } else if(argv[3][0] == '5') { //碳（石墨）
                objcolor = F0 = glm::vec3(0.04f, 0.04f, 0.04f);
                metallic = 0.0f;
                albedo = glm::vec3(0.05f, 0.05f, 0.05f);
                roughness = 0.8f;
            } else if(argv[3][0] == '6') { //金
                objcolor = F0 = glm::vec3(1.022f, 0.782f, 0.344f);
                metallic = 1.0f;
                albedo = glm::vec3(1.0f, 0.85f, 0.57f);
                roughness = 0.3f;
            } else if(argv[3][0] == '7') { //锡
                objcolor = F0 = glm::vec3(0.549f, 0.556f, 0.554f);
                metallic = 1.0f;
                albedo = glm::vec3(0.75f, 0.75f, 0.75f);
                roughness = 0.25f;
            }
        }
        if(argc > 4) {
            roughness = std::stof(argv[4]);
        }
    }

    if(argc > 1 && (std::string)argv[1] == "dinosaur") {
        viewPos = glm::vec3(0.0f, 0.0f, 150.0f);
        lightPos = glm::vec3(40.0f, 40.0f, 40.0f);
        rot = glm::rotate(rot, 4.7f, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    if(argc > 1 && (std::string)argv[1] == "cube") {
        viewPos = glm::vec3(0.0f, 0.2f, 4.0f);
    }
    if(argc > 1 && (std::string)argv[1] == "pyramid") {
        lightPos = glm::vec3(4.0f, 4.0f, 4.0f);
        lightcolor = glm::vec3(0.8f, 0.8f, 0.8f);
    }
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        float time = static_cast<float>(glfwGetTime());
        // 视口尺寸
        int fbw, fbh;
        glfwGetFramebufferSize(window, &fbw, &fbh);
        float aspect = (fbh == 0) ? 1.0f : (static_cast<float>(fbw) / static_cast<float>(fbh));

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));
        model = model * rot;
        glm::mat4 view  = glm::translate(glm::mat4(1.0f), -viewPos);
        view = glm::rotate(view, 0.4f, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 proj  = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 300.0f);
        glm::mat4 mvp   = proj * view * model;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制模型
        shader->use();
        shader->setMat4("uMVP", mvp);
        shader->setMat4("uModel", model);

        shader->setVec3("uLightPos", lightPos);
        shader->setVec3("uViewPos", viewPos);
        shader->setVec3("uLightColor", lightcolor);
        shader->setVec3("uObjectColor", objcolor);
        shader->setFloat("uAmbient", ambient);
        shader->setFloat("uSpecular", specular);
        shader->setFloat("uShininess", shininess);
        shader->setFloat("uMetallic", metallic);
        // Cook-Torrance uniforms (no-op for other shaders)
        shader->setVec3("F0", F0);
        shader->setVec3("uAlbedo", albedo);
        shader->setFloat("uRoughness", roughness);
        glBindVertexArray(VAO); 
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    // glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
#include <glad/glad.h>
#include <glfw3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "../../shaders/include/Shader.h"
#include "../../scene/include/Mesh.h"
#include "../../mathtool/include/MathType.h"
#include "../../scene/include/Model.h"

// Mat4 perspective(float fov, float aspect, float nearPlane, float farPlane) {
//     Mat4 r = {};

//     float t = tan(fov / 2.0f * PI / 180.f);
//     r.m[0] = 1.0f / (aspect * t);
//     r.m[5] = 1.0f / t;
//     r.m[10] = -(nearPlane + farPlane) / (farPlane - nearPlane);
//     r.m[11] = -1.0f;
//     r.m[14] = -(2 * nearPlane * farPlane) / (farPlane - nearPlane);

//     return r;
// }

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    Shader shader("../../shaders/vertexshaders/simple.shader", "../../shaders/fragmentshaders/simple.shader");

    // std::vector<float> verticies = {
    //     // 底面（z = 0）
    //     -0.5f, -0.5f,  0.0f,
    //     0.5f, -0.5f,  0.0f,
    //     0.5f,  0.5f,  0.0f,
    //     -0.5f,  0.5f,  0.0f,

    //     // 顶点
    //     0.0f,  0.0f,  0.8f
    // };

    // std::vector<unsigned int> indices = {
    //     // 底面
    //     0, 1, 2,
    //     0, 2, 3,

    //     // 四个侧面
    //     0, 1, 4,
    //     1, 2, 4,
    //     2, 3, 4,
    //     3, 0, 4
    // };
    Model connelBox;
    connelBox.LoadOBJ("../../assets/ConnelBox.obj");
    std::vector<Mesh> meshes = connelBox.GetMeshes();
    // Mesh mesh(verticies, indices);

    Mat4 view = Mat4::getViewMat({-0.2, 2.5, 5.f}, {-0.2, 2.5, -2.5}, {0, 1, 0});
    Mat4 model = Mat4::identity();
    Mat4 proj = Mat4::getPerspectiveMat(45.f, 800.f/600.f, 0.1f, 100.f);

    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    for (Mesh& mesh : meshes) {
        mesh.uploadToGPU();
    }

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", proj);
        shader.setVec3("cameraPos", {0, 0, 2});
        shader.setVec3("lightPos", {0.204011, 5.3189155, -3.042968});
        shader.setVec3("lightColor", {1.0f, 1.0f, 1.0f});
        for (Mesh& mesh : meshes) {
            if (mesh.name == "area_light")
                shader.setVec3("objectColor", {1.0f, 1.0f, 1.0f});
            else if (mesh.name == "left_wall")
                shader.setVec3("objectColor", {0.65, 0.05, 0.05});
            else if (mesh.name == "right_wall")
                shader.setVec3("objectColor", {0.0, 0.39, 0.0});
            else 
                shader.setVec3("objectColor", {0.76, 0.69, 0.57});
            mesh.draw();
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
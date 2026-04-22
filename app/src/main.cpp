#include <glad/glad.h>
#include <glfw3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "../../shaders/include/Shader.h"
#include "../../mesh/include/Mesh.h"
#include "../../math/include/MathType.h"

Mat4 perspective(float fov, float aspect, float nearPlane, float farPlane) {
    Mat4 r = {};

    float t = tan(fov / 2.0f * PI / 180.f);
    r.m[0] = 1.0f / (aspect * t);
    r.m[5] = 1.0f / t;
    r.m[10] = -(nearPlane + farPlane) / (farPlane - nearPlane);
    r.m[11] = -1.0f;
    r.m[14] = -(2 * nearPlane * farPlane) / (farPlane - nearPlane);

    return r;
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    Shader shader("../../shaders/vertexshaders/simple.shader", "../../shaders/fragmentshaders/simple.shader");

    std::vector<float> verticies = {
        // 底面（z = 0）
        -0.5f, -0.5f,  0.0f,
        0.5f, -0.5f,  0.0f,
        0.5f,  0.5f,  0.0f,
        -0.5f,  0.5f,  0.0f,

        // 顶点
        0.0f,  0.0f,  0.8f
    };

    std::vector<unsigned int> indices = {
        // 底面
        0, 1, 2,
        0, 2, 3,

        // 四个侧面
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    Mesh mesh(verticies, indices);

    Mat4 view = Mat4::getViewMat({0, 0, 2}, {0, 0, -0.9}, {0, 1, 0});
    Mat4 model = Mat4::identity();
    Mat4 proj = Mat4::getPerspectiveMat(45.f, 800.f/600.f, 0.1f, 100.f);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", proj);
        mesh.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
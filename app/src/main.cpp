#include <glad/glad.h>
#include <glfw3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include <map>

#include "../../shaders/include/Shader.h"
#include "../../scene/include/Mesh.h"
#include "../../mathtool/include/MathType.h"
#include "../../scene/include/Model.h"
#include "../../scene/include/Texture.h"
#include "../../scene/include/Camera.h"

Vec3 cam_forward = {0.f, 0.f, 1.f};
float cam_fov = 45.f;

void pressInput(GLFWwindow* window, Camera& cam) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
        cam.position = cam.position + cam.forward * cam.speed;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.position = cam.position - cam.forward * cam.speed;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.position = cam.position - cam.forward.crossProduct(cam.up).normalize() * cam.speed;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.position = cam.position + cam.forward.crossProduct(cam.up).normalize() * cam.speed;
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
    static bool firstMouse = true;
    static float lastX = 400.f;
    static float lastY = 300.f;
    static float yaw = 0.f;
    static float pitch = 0.f;

    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = yPos - lastY;
    lastX = xPos;
    lastY = yPos;

    float sensitivity = 0.05f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    pitch = std::max(pitch, -89.f);
    pitch = std::min(pitch, 89.f);

    float yaw_rad = yaw * PI / 180.f;
    float pitch_rad = pitch * PI / 180.f;

    cam_forward.x = std::cos(yaw_rad) * std::cos(pitch_rad);
    cam_forward.y = std::sin(pitch_rad);
    cam_forward.z = std::sin(yaw_rad) * std::cos(pitch_rad);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    if (cam_fov >= 1.f && cam_fov <= 45.f) 
        cam_fov -= yOffset;
    cam_fov = std::max(cam_fov, 1.f);
    cam_fov = std::min(cam_fov, 45.f);
}

int main() {
    if (!glfwInit()) return -1;

    // 指定主次版本号、以及使用核心模式
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);

    // glad 用于管理opengl函数指针，调用opengl函数前需要初始化函数指针
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    Shader shader("../../shaders/vertexshaders/simple.shader", "../../shaders/fragmentshaders/simple.shader");

    float points[] = {
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f  
    };

    Vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y=-10; y<10; y+=2) {
        for (int x=-10; x<10; x+=2) {
            Vec2 translation;
            translation.x = (float)x / 10.f + offset;
            translation.y = (float)y / 10.f + offset;
            translations[index++] = translation;
        }
    }

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 创建VBO，用于存储VertexBuffer数据
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 5 * 4, points, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 20, (void*)8);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetCursorPosCallback(window, mouseCallback);
    // glfwSetScrollCallback(window, scrollCallback);

    float deltatime = 0.0f;
    float lastFrame = 0.0f;

    // glfwWindowShouldClose检查一次GLFW是否被要求退出，是就返回true，就停止循环，然后可以关闭应用程序
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        shader.use();
        for (int i=0; i<100; i++)
            shader.setVec2("offsets[" + std::to_string(i) + "]", translations[i]);
        // glDrawArrays(GL_POINTS, 0, 4);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
        
        // glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。
        glfwSwapBuffers(window);
        // glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
        glfwPollEvents();
    }
    // 释放/删除之前的分配的所有资源
    glfwTerminate();
}
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

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
            std::cout << "OpenGL Error: " << message << std::endl;
        },
        nullptr
    );

    Shader shader("../../shaders/vertexshaders/simple.shader", "../../shaders/fragmentshaders/simple.shader");
    Shader screenShader("../../shaders/vertexshaders/screen.shader", "../../shaders/fragmentshaders/screen.shader");

    float points[] = {
        -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   
         0.5f,  0.5f,  0.0f, 1.0f, 1.0f  
    };

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

    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // 创建framebuffer
    unsigned int framebuffer;
    glGenFramebuffers(1, & framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // 创建纹理并将纹理附加到framebuffer上
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, 800, 600, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex, 0);

    // 创建renderbuffer
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // 将renderbuffer附加到framebuffer上
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int itmdFBO;
    glGenFramebuffers(1, &itmdFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, itmdFBO);

    unsigned int screenTexture;
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Mat4 model = Mat4::getRotateMat({0.f, 0.f, 45.f});

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetCursorPosCallback(window, mouseCallback);
    // glfwSetScrollCallback(window, scrollCallback);

    float deltatime = 0.0f;
    float lastFrame = 0.0f;

    // glfwWindowShouldClose检查一次GLFW是否被要求退出，是就返回true，就停止循环，然后可以关闭应用程序
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        shader.use();
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, itmdFBO);
        glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        screenShader.setInt("screenTexture", 0);
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
        
        // glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。
        glfwSwapBuffers(window);
        // glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
        glfwPollEvents();
    }
    // 释放/删除之前的分配的所有资源
    glfwTerminate();
}
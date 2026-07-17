#include <glad/glad.h>
#include <glfw3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>

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

    // 指示每个屏幕坐标使用4个采样点的颜色缓冲
    glfwWindowHint(GLFW_SAMPLES, 4);

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

    Shader shader("../../shaders/vertexshaders/simple.shader", "../../shaders/fragmentshaders/simple.shader", "../../shaders/geometryshaders/simple.shader");
    Shader normalShader("../../shaders/vertexshaders/normal.shader", "../../shaders/fragmentshaders/normal.shader", "../../shaders/geometryshaders/normal.shader");
    Shader lightShader("../../shaders/vertexshaders/light.shader", "../../shaders/fragmentshaders/light.shader");
    Shader singleColorShader("../../shaders/vertexshaders/simple.shader", "../../shaders/fragmentshaders/singleColor.shader");
    Shader planetShader("../../shaders/vertexshaders/planet.shader", "../../shaders/fragmentshaders/planet.shader");
    Shader rockShader("../../shaders/vertexshaders/rock.shader", "../../shaders/fragmentshaders/rock.shader");

    Camera cam;
    cam.position = {0.f, 0.f, 3.f};
    cam.forward = {0.f, 0.f, 1.f};
    cam.up = {0.f, 1.f, 0.f};
    cam.fov = 45.f;
    cam.aspect = 800.f / 600.f;
    cam.nearPlane = 0.1f;
    cam.farPlane = 100.f;
    cam.speed = 0.05f;

    Model model("../../assets/backpack/backpack.obj");
    Model planet("../../assets/planet/planet.obj");
    Model rock("../../assets/rock/rock.obj");
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    Mat4 positions[] = {
        Mat4::getTranslateMat({-1.5f, -2.2f, -2.5f}),
        Mat4::getTranslateMat({-1.3f,  1.0f, -1.5f}),
    };

    std::vector<Vec3> vegetation = {
        {-1.5f, 0.0f, -0.48f},
        { 1.5f, 0.0f,  0.51f},
        { 0.0f, 0.0f,  0.7f},
        {-0.3f, 0.0f, -2.3f},
        { 0.5f, 0.0f, -0.6f},
    };

    Vec3 lightPositions[] = {
        {0.7f,  0.2f,  2.0f},
        {2.3f, -3.3f, -4.0f},
        {-4.0f,  2.0f, -12.0f},
        {0.0f,  0.0f, -3.0f}
    };

    unsigned int lightVAO, lightVBO;
    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, 36 * 8 * 4, vertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_MULTISAMPLE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    float deltatime = 0.0f;
    float lastFrame = 0.0f;

    // Vec3 lightPos({1.2f, 1.0f, 2.0f});
    Mat4 lightModels[] = {
        Mat4::getTranslateMat(lightPositions[0]) * Mat4::getScaleMat({0.2f, 0.2f, 0.2f}),
        Mat4::getTranslateMat(lightPositions[1]) * Mat4::getScaleMat({0.2f, 0.2f, 0.2f}),
        Mat4::getTranslateMat(lightPositions[2]) * Mat4::getScaleMat({0.2f, 0.2f, 0.2f}),
        Mat4::getTranslateMat(lightPositions[3]) * Mat4::getScaleMat({0.2f, 0.2f, 0.2f}),
    };

    unsigned int amount = 12000;
    std::vector<Mat4> rockModelMatrixes(amount);
    srand(glfwGetTime());
    float radius = 25.f;
    float offset = 10.f;
    for (unsigned int i=0; i<amount; i++) {
        // 位移
        float angle = (float)i / (float)amount * 360.f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;
        float x = std::sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;
        float y = displacement * 0.4;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset;
        float z = cos(angle) * radius + displacement;
        // 缩放
        float scale = (rand() % 20) / 100.f + 0.05;
        // 旋转
        float rotAngle = (rand() % 20) / 100.f + 0.05;
        Mat4 rockModel = Mat4::getTranslateMat({x, y, z}) * Mat4::getRotateMat({0.4f * rotAngle, 0.6f * rotAngle, 0.8f * rotAngle}) * Mat4::getScaleMat({scale, scale, scale});
        rockModelMatrixes[i] = rockModel;
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(Mat4), rockModelMatrixes.data(), GL_STATIC_DRAW);

    std::vector<Mesh>& rockMeshes = rock.getMeshse();
    for (unsigned int i=0; i<rockMeshes.size(); i++) {
        glBindVertexArray(rockMeshes[i].getVAO());
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vec4), (void*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vec4), (void*)sizeof(Vec4));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vec4), (void*)(sizeof(Vec4) * 2));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vec4), (void*)(sizeof(Vec4) * 3));
        glEnableVertexAttribArray(6);

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }


    // glfwWindowShouldClose检查一次GLFW是否被要求退出，是就返回true，就停止循环，然后可以关闭应用程序
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        float currentFrame = glfwGetTime();
        deltatime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        cam.speed = 2.5 * deltatime;
        cam.forward = cam_forward.normalize();
        cam.fov = cam_fov;
        pressInput(window, cam);
        Mat4 view = cam.getViewMat();
        Mat4 proj = cam.getProjectionMat();
        shader.use();
        // shader.setInt("material.diffuse", 0);
        // shader.setInt("material.specular", 1);

        float curTime = glfwGetTime();
        // shader.setVec3("lightColor", {sin(curTime * 2.0f), sin(curTime * 0.3f), sin(curTime * 1.7f)});
        // shader.setVec3("lightColor", {1.0f, 1.0f, 1.0f});
        // // shader.setMat4("model", model);
        // shader.setMat4("view", view);
        // shader.setMat4("projection", proj);
        // shader.setVec3("cameraPos", cam.position);

        // shader.setFloat("time", currentFrame);

        // shader.setVec3("material.ambient", {1.0f, 0.5f, 0.31f});
        // shader.setVec3("material.diffuse", {1.0f, 0.5f, 0.31f});
        // // shader.setVec3("material.specular", {0.5f, 0.5f, 0.5f});
        // shader.setFloat("material.shininess", 32.0f);
        // // 聚光灯
        // shader.setVec3("spotLight.ambient", {0.2f, 0.2f, 0.2f});
        // shader.setVec3("spotLight.diffuse", {0.5f, 0.5f, 0.5f});
        // shader.setVec3("spotLight.specular", {1.0f, 1.0f, 1.0f});
        // shader.setVec3("spotLight.position", cam.position);
        // shader.setVec3("spotLight.direction", cam.forward);
        // shader.setFloat("spotLight.outterCutOff", cos(17.5 * PI / 180));
        // shader.setFloat("spotLight.cutOff", cos(12.5 * PI / 180));
        
        // shader.setFloat("spotLight.constant", 1.0f);
        // shader.setFloat("spotLight.linear", 0.09f);
        // shader.setFloat("spotLight.quadratic", 0.032f);
        // // 平行光、太阳光
        // shader.setVec3("dirLight.ambient", {0.2f, 0.2f, 0.2f});
        // shader.setVec3("dirLight.diffuse", {0.5f, 0.5f, 0.5f});
        // shader.setVec3("dirLight.specular", {1.0f, 1.0f, 1.0f});
        // shader.setVec3("dirLight.direction", {-1.0f, -1.0f, -1.0f});
        // // 其他点光源
        // shader.setVec3("pointLights[0].ambient", {0.2f, 0.2f, 0.2f});
        // shader.setVec3("pointLights[0].diffuse", {0.5f, 0.5f, 0.5f});
        // shader.setVec3("pointLights[0].specular", {1.0f, 1.0f, 1.0f});
        // shader.setVec3("pointLights[0].position", lightPositions[0]);

        // shader.setFloat("pointLights[0].constant", 1.0f);
        // shader.setFloat("pointLights[0].linear", 0.09f);
        // shader.setFloat("pointLights[0].quadratic", 0.032f);

        // shader.setVec3("pointLights[1].ambient", {0.2f, 0.2f, 0.2f});
        // shader.setVec3("pointLights[1].diffuse", {0.5f, 0.5f, 0.5f});
        // shader.setVec3("pointLights[1].specular", {1.0f, 1.0f, 1.0f});
        // shader.setVec3("pointLights[1].position", lightPositions[1]);

        // shader.setFloat("pointLights[1].constant", 1.0f);
        // shader.setFloat("pointLights[1].linear", 0.09f);
        // shader.setFloat("pointLights[1].quadratic", 0.032f);

        // shader.setVec3("pointLights[2].ambient", {0.2f, 0.2f, 0.2f});
        // shader.setVec3("pointLights[2].diffuse", {0.5f, 0.5f, 0.5f});
        // shader.setVec3("pointLights[2].specular", {1.0f, 1.0f, 1.0f});
        // shader.setVec3("pointLights[2].position", lightPositions[2]);

        // shader.setFloat("pointLights[2].constant", 1.0f);
        // shader.setFloat("pointLights[2].linear", 0.09f);
        // shader.setFloat("pointLights[2].quadratic", 0.032f);

        // shader.setVec3("pointLights[3].ambient", {0.2f, 0.2f, 0.2f});
        // shader.setVec3("pointLights[3].diffuse", {0.5f, 0.5f, 0.5f});
        // shader.setVec3("pointLights[3].specular", {1.0f, 1.0f, 1.0f});
        // shader.setVec3("pointLights[3].position", lightPositions[3]);

        // shader.setFloat("pointLights[3].constant", 1.0f);
        // shader.setFloat("pointLights[3].linear", 0.09f);
        // shader.setFloat("pointLights[3].quadratic", 0.032f);

        // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // glStencilFunc(GL_ALWAYS, 1, 0xff);
        // glStencilMask(0xFF);
        // for (int i=0; i<2; i++) {
        //     Mat4 rotateMat = Mat4::getRotateMat({5.f * (i + 1), 10.f * (i + 1), 0.f});
        //     Mat4 modelMat = positions[i] * rotateMat;
        //     shader.setMat4("model", modelMat);
        //     model.Draw(shader);
        //     // glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        // glStencilFunc(GL_NOTEQUAL, 1, 0xff);
        // glStencilMask(0x00);
        // glDisable(GL_DEPTH_TEST);
        // singleColorShader.use();
        // singleColorShader.setMat4("view", view);
        // singleColorShader.setMat4("projection", proj);
        // for (int i=0; i<2; i++) {
        //     Mat4 scaleMat = Mat4::getScaleMat({1.1, 1.1, 1.1});
        //     Mat4 rotateMat = Mat4::getRotateMat({5.f * (i + 1), 10.f * (i + 1), 0.f});
        //     Mat4 modelMat = positions[i] * rotateMat * scaleMat;
        //     singleColorShader.setMat4("model", modelMat);
        //     model.Draw(singleColorShader);
        // }

        // glStencilMask(0xff);
        // glEnable(GL_DEPTH_TEST);
        // lightShader.use();
        // // lightShader.setMat4("model", lightModel);
        // lightShader.setMat4("view", view);
        // lightShader.setMat4("projection", proj);
        // lightShader.setVec3("LightColor", {1.0f, 1.0f, 1.0f});

        // for (int i=0; i<4; i++) {
        //     lightShader.setMat4("model", lightModels[i]);
        //     glBindVertexArray(lightVAO);
        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        // normalShader.use();
        // normalShader.setMat4("view", view);
        // normalShader.setMat4("projection", proj);

        // for (int i=0; i<2; i++) {
        //     Mat4 rotateMat = Mat4::getRotateMat({5.f * (i + 1), 10.f * (i + 1), 0.f});
        //     Mat4 modelMat = positions[i] * rotateMat;
        //     normalShader.setMat4("model", modelMat);
        //     model.Draw(normalShader);
        //     // glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        planetShader.use();
        planetShader.setMat4("view", view);
        planetShader.setMat4("projection", proj);
        planetShader.setMat4("model", Mat4::getScaleMat({2.0, 2.0, 2.0}));
        planet.Draw(planetShader);

        rockShader.use();
        rockShader.setMat4("view", view);
        rockShader.setMat4("projection", proj);
        rock.DrawInstances(rockShader, amount);
        // for (int i=0; i<amount; i++) {
        //     rockShader.setMat4("model", rockModelMatrixes[i]);
        //     rock.Draw(rockShader);
        // }
        
        
        // glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。
        glfwSwapBuffers(window);
        // glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
        glfwPollEvents();
    }
    // 释放/删除之前的分配的所有资源
    glfwTerminate();
}
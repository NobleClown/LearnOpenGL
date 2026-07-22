#include <glad/glad.h>
#include <glfw3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include <map>
#include <random>

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
    Shader lightShader("../../shaders/vertexshaders/light.shader", "../../shaders/fragmentshaders/light.shader");
    Shader windowShader("../../shaders/vertexshaders/window.shader", "../../shaders/fragmentshaders/window.shader");
    Shader screenShader("../../shaders/vertexshaders/screen.shader", "../../shaders/fragmentshaders/screen.shader");
    Shader skyboxShader("../../shaders/vertexshaders/skybox.shader", "../../shaders/fragmentshaders/skybox.shader");
    Shader reflectShader("../../shaders/vertexshaders/reflect.shader", "../../shaders/fragmentshaders/reflect.shader");
    Shader hdrShader("../../shaders/vertexshaders/hdr.shader", "../../shaders/fragmentshaders/hdr.shader");
    Shader blurShader("../../shaders/vertexshaders/blur.shader", "../../shaders/fragmentshaders/blur.shader");
    Shader gbufferShader("../../shaders/vertexshaders/gbuffer.shader", "../../shaders/fragmentshaders/gbuffer.shader");
    Shader lightingpassShader("../../shaders/vertexshaders/lightingpass.shader", "../../shaders/fragmentshaders/lightingpass.shader");
    Shader ssaoShader("../../shaders/vertexshaders/ssao.shader", "../../shaders/fragmentshaders/ssao.shader");
    Shader ssaoblurShader("../../shaders/vertexshaders/ssaoblur.shader", "../../shaders/fragmentshaders/ssaoblur.shader");

    unsigned int uniformBlockIndexShader = glGetUniformBlockIndex(shader.getProgramID(), "Matrices");
    unsigned int uniformBlockIndexLightShader = glGetUniformBlockIndex(lightShader.getProgramID(), "Matrices");
    unsigned int uniformBlockIndexWindowShader = glGetUniformBlockIndex(windowShader.getProgramID(), "Matrices");
    unsigned int uniformBlockIndexReflectShader = glGetUniformBlockIndex(reflectShader.getProgramID(), "Matrices");

    glUniformBlockBinding(shader.getProgramID(), uniformBlockIndexShader, 0);
    glUniformBlockBinding(lightShader.getProgramID(), uniformBlockIndexLightShader, 0);
    glUniformBlockBinding(windowShader.getProgramID(), uniformBlockIndexWindowShader, 0);
    glUniformBlockBinding(reflectShader.getProgramID(), uniformBlockIndexReflectShader, 0);

    unsigned int uboMatrices;
    glGenBuffers(1, & uboMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(Mat4));

    // Model connelBox;
    // connelBox.LoadOBJ("../../assets/ConnelBox.obj");
    // std::vector<Mesh> meshes = connelBox.GetMeshes();

    // Mat4 view = Mat4::getViewMat({0, 0, -20}, {0.f, 0.f, 0.f}, {0, 1, 0});
    // Mat4 model = Mat4::getRotateMat({55.f, 0.f, 0.f});
    // Mat4 model = Mat4::identity();

    Camera cam;
    cam.position = {0.f, 0.f, 3.f};
    cam.forward = {0.f, 0.f, 1.f};
    cam.up = {0.f, 1.f, 0.f};
    cam.fov = 45.f;
    cam.aspect = 800.f / 600.f;
    cam.nearPlane = 0.1f;
    cam.farPlane = 100.f;
    cam.speed = 0.05f;

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 

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
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  
    };

    float tangent[108] = {0};

    int vtx_cnt = 36;
    for (int i=0; i<12; i++) {
        Vec3 v1Pos = {vertices[i*3*8], vertices[i*3*8+1], vertices[i*3*8+2]};
        Vec3 v2Pos = {vertices[(i*3+1)*8], vertices[(i*3+1)*8+1], vertices[(i*3+1)*8+2]};
        Vec3 v3Pos = {vertices[(i*3+2)*8], vertices[(i*3+2)*8+1], vertices[(i*3+2)*8+2]};

        Vec2 v1UV = {vertices[i*3*8+6], vertices[i*3*8+7]};
        Vec2 v2UV = {vertices[(i*3+1)*8+6], vertices[(i*3+1)*8+7]};
        Vec2 v3UV = {vertices[(i*3+2)*8+6], vertices[(i*3+2)*8+7]};
        Vec3 edge1 = v2Pos - v1Pos;
        Vec3 edge2 = v3Pos - v1Pos;

        Vec2 deltaUV1 = v2UV - v1UV;
        Vec2 deltaUV2 = v3UV - v1UV;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        Vec3 tangentVec = {
            f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
            f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
            f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z),
        };
        int t = i / 2;
        if (t % 2 == 0)
            tangentVec = tangentVec * (-1);
        tangent[i*9] = tangentVec.x;
        tangent[i*9+1] = tangentVec.y;
        tangent[i*9+2] = tangentVec.z;
        tangent[i*9+3] = tangentVec.x;
        tangent[i*9+4] = tangentVec.y;
        tangent[i*9+5] = tangentVec.z;
        tangent[i*9+6] = tangentVec.x;
        tangent[i*9+7] = tangentVec.y;
        tangent[i*9+8] = tangentVec.z;
    }

    float planeVertices[] = {
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    float screenVertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,

        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    Vec3 planePositions[] = {
        {-1.5f, 0.0f, -0.48f},
        { 1.5f, 0.1f,  0.51f},
        { 0.0f, 0.2f,  0.7f},
        {-0.3f, 0.3f, -2.3f},
        { 0.5f, 0.4f, -0.6f},
    };

    Mat4 positions[] = {
        Mat4::getTranslateMat({0.f, 0.f, 0.f}),
        Mat4::getTranslateMat({2.f, 5.f, -15.f}),
        Mat4::getTranslateMat({-1.5f, -2.2f, -2.5f}),
        Mat4::getTranslateMat({-3.8f, -2.0f, -12.3f}),
        Mat4::getTranslateMat({2.4f, -0.4f, -3.5f}),
        Mat4::getTranslateMat({-1.7f,  3.0f, -7.5f}),
        Mat4::getTranslateMat({1.3f, -2.0f, -2.5f}),
        Mat4::getTranslateMat({1.5f,  2.0f, -2.5f}),
        Mat4::getTranslateMat({1.5f,  0.2f, -1.5f}),
        Mat4::getTranslateMat({-1.3f,  1.0f, -1.5f}),
    };

    Vec3 lightPositions[] = {
        {0.7f,  0.2f,  2.0f},
        {2.3f, -3.3f, -4.0f},
        {-4.0f,  2.0f, -12.0f},
        {0.0f,  0.0f, -3.0f}
    };

    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    std::vector<Vec3> ssaoKernel;
    for (int i=0; i<64; i++) {
        Vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = sample.normalize();
        sample = sample * randomFloats(generator);
        float scale = i / 64.0;
        scale = 0.1 + scale * scale * 0.9;
        ssaoKernel.push_back(sample * scale);
    }

    std::vector<Vec3> ssaoNoise;
    for (int i=0; i<16; i++) {
        Vec3 noise (
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0
        );
        ssaoNoise.push_back(noise);
    }

    // unsigned int indices[] = {0, 1, 2, 0, 2, 3};

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 创建VBO，用于存储VertexBuffer数据
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 36 * 8 * 4, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (void*)12);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void*)24);
    glEnableVertexAttribArray(2);

    unsigned int tanVBO;
    glGenBuffers(1, &tanVBO);
    glBindBuffer(GL_ARRAY_BUFFER, tanVBO);
    glBufferData(GL_ARRAY_BUFFER, 36 * 3 * 4, tangent, GL_STATIC_DRAW);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    unsigned int windowVAO, windowVBO;

    glGenVertexArrays(1, &windowVAO);
    glBindVertexArray(windowVAO);

    glGenBuffers(1, &windowVBO);
    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 5 * 4, planeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    unsigned int skyboxVAO, skyboxVBO;

    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);

    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, 36 * 3 * 4, skyboxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glCullFace(GL_BACK);
    // for (Mesh& mesh : meshes) {
    //     mesh.uploadToGPU();
    // }

    unsigned int screenVAO, screenVBO;
    glGenVertexArrays(1, &screenVAO);
    glBindVertexArray(screenVAO);

    glGenBuffers(1, &screenVBO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 5 * 4, screenVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, & quadVAO);
    glBindVertexArray(quadVAO);

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * 5 * 4, quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    std::vector<std::string> skyboxImagePaths = {
        "../../assets/skybox/right.jpg", 
        "../../assets/skybox/left.jpg", 
        "../../assets/skybox/top.jpg", 
        "../../assets/skybox/bottom.jpg", 
        "../../assets/skybox/front.jpg", 
        "../../assets/skybox/back.jpg", 
    };

    Texture t_box, t_spec, t_window;
    t_box.loadTexture("../../assets/container2.png", 0);
    t_spec.loadTexture("../../assets/matrix.jpg", 1);
    t_window.loadTexture("../../assets/blending_transparent_window.png", 2);

    CubeTexture t_skybox;
    t_skybox.loadTexture(skyboxImagePaths, 4);

    Texture t_brick, t_brickNorm, t_brickDisp;
    t_brick.loadTexture("../../assets/bricks2.jpg", 5);
    t_brickNorm.loadTexture("../../assets/bricks2_normal.jpg", 6);
    t_brickDisp.loadTexture("../../assets/bricks2_disp.jpg", 7);

    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    unsigned int gPosition, gNormal, gColorSpec;

    // 位置buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    glGenTextures(1, &gColorSpec);
    glBindTexture(GL_TEXTURE_2D, gColorSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER::framebuffer is not complete!" << std::endl;
    }

    glActiveTexture(GL_TEXTURE0 + 8);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE0 + 9);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE0 + 10);
    glBindTexture(GL_TEXTURE_2D, gColorSpec);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    unsigned int ssaoFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int ssaoColorBuffer;
    glGenTextures(1, &ssaoColorBuffer);
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 800, 600, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

    unsigned int ssaoBlurFBO, ssaoBlurColorBuffer;
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

    glGenTextures(1, &ssaoBlurColorBuffer);
    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 800, 600, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColorBuffer, 0);

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

    Mat4 windowModels[] = {
        Mat4::getTranslateMat(planePositions[0]),
        Mat4::getTranslateMat(planePositions[1]),
        Mat4::getTranslateMat(planePositions[2]),
        Mat4::getTranslateMat(planePositions[3]),
        Mat4::getTranslateMat(planePositions[4]),
    };

    float constant = 1.0;
    float linear = 0.7;
    float quadratic = 1.8;
    float lightMAx = 1.0;
    float radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic *(constant - (256.0 / 5.0) * lightMAx)));

    // glfwWindowShouldClose检查一次GLFW是否被要求退出，是就返回true，就停止循环，然后可以关闭应用程序
    while (!glfwWindowShouldClose(window)) {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        // t1.setTexture();
        // float radius = 10.f;
        // float camX = sin(glfwGetTime()) * radius;
        // float camZ = cos(glfwGetTime()) * radius;
        // Mat4 view = Mat4::getViewMat({camX, 0, camZ}, {0.f, 0.f, 0.f}, {0, 1, 0});
        float currentFrame = glfwGetTime();
        deltatime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        cam.speed = 2.5 * deltatime;
        cam.forward = cam_forward.normalize();
        cam.fov = cam_fov;
        pressInput(window, cam);
        Mat4 view = cam.getViewMat();
        Mat4 viewSkybox = cam.getViewMat();
        viewSkybox.set(0, 3, 0);
        viewSkybox.set(1, 3, 0);
        viewSkybox.set(2, 3, 0);
        Mat4 proj = cam.getProjectionMat();
        
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Mat4), (void *)(proj.m));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Mat4), sizeof(Mat4), (void *)(view.m));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindVertexArray(VAO);

        gbufferShader.use();
        gbufferShader.setInt("texture_diffuse1", 0);
        gbufferShader.setInt("texture_specular1", 1);

        for (int i=0; i<10; i++) {
            Mat4 rotateMat = Mat4::getRotateMat({5.f * (i + 1), 10.f * (i + 1), 0.f});
            Mat4 model = positions[i] * rotateMat;
            // shader.setMat4("model", model);
            gbufferShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoShader.use();
        ssaoShader.setInt("gPositionDepth", 8);
        ssaoShader.setInt("gNormal", 9);
        ssaoShader.setInt("texNoise", 11);
        ssaoShader.setMat4("projection", proj);
        for (int i=0; i<ssaoKernel.size(); i++)
            ssaoShader.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        ssaoblurShader.use();
        ssaoblurShader.setInt("ssaoInput", 12);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingpassShader.use();
        lightingpassShader.setVec3("spotLight.ambient", {0.2f, 0.2f, 0.2f});
        lightingpassShader.setVec3("spotLight.diffuse", {0.5f, 0.5f, 0.5f});
        lightingpassShader.setVec3("spotLight.specular", {1.0f, 1.0f, 1.0f});
        lightingpassShader.setVec3("spotLight.position", cam.position);
        lightingpassShader.setVec3("spotLight.direction", cam.forward);
        lightingpassShader.setFloat("spotLight.outterCutOff", cos(17.5 * PI / 180));
        lightingpassShader.setFloat("spotLight.cutOff", cos(12.5 * PI / 180));
        
        lightingpassShader.setFloat("spotLight.constant", 1.0f);
        lightingpassShader.setFloat("spotLight.linear", 0.09f);
        lightingpassShader.setFloat("spotLight.quadratic", 0.032f);
        // 平行光、太阳光
        lightingpassShader.setVec3("dirLight.ambient", {0.2f, 0.2f, 0.2f});
        lightingpassShader.setVec3("dirLight.diffuse", {0.5f, 0.5f, 0.5f});
        lightingpassShader.setVec3("dirLight.specular", {1.0f, 1.0f, 1.0f});
        lightingpassShader.setVec3("dirLight.direction", {-1.0f, -1.0f, -1.0f});
        // 其他点光源
        lightingpassShader.setVec3("pointLights[0].ambient", {0.2f, 0.2f, 0.2f});
        lightingpassShader.setVec3("pointLights[0].diffuse", {0.5f, 0.5f, 0.5f});
        lightingpassShader.setVec3("pointLights[0].specular", {1.0f, 1.0f, 1.0f});
        lightingpassShader.setVec3("pointLights[0].position", lightPositions[0]);
        lightingpassShader.setFloat("pointLights[0].constant", 1.0f);
        lightingpassShader.setFloat("pointLights[0].linear", 0.09f);
        lightingpassShader.setFloat("pointLights[0].quadratic", 0.032f);
        lightingpassShader.setFloat("pointLights[0].Radius", radius);

        lightingpassShader.setVec3("pointLights[1].ambient", {0.2f, 0.2f, 0.2f});
        lightingpassShader.setVec3("pointLights[1].diffuse", {0.5f, 0.5f, 0.5f});
        lightingpassShader.setVec3("pointLights[1].specular", {1.0f, 1.0f, 1.0f});
        lightingpassShader.setVec3("pointLights[1].position", lightPositions[1]);
        lightingpassShader.setFloat("pointLights[1].constant", 1.0f);
        lightingpassShader.setFloat("pointLights[1].linear", 0.09f);
        lightingpassShader.setFloat("pointLights[1].quadratic", 0.032f);
        lightingpassShader.setFloat("pointLights[1].Radius", radius);

        lightingpassShader.setVec3("pointLights[2].ambient", {0.2f, 0.2f, 0.2f});
        lightingpassShader.setVec3("pointLights[2].diffuse", {0.5f, 0.5f, 0.5f});
        lightingpassShader.setVec3("pointLights[2].specular", {1.0f, 1.0f, 1.0f});
        lightingpassShader.setVec3("pointLights[2].position", lightPositions[2]);
        lightingpassShader.setFloat("pointLights[2].constant", 1.0f);
        lightingpassShader.setFloat("pointLights[2].linear", 0.09f);
        lightingpassShader.setFloat("pointLights[2].quadratic", 0.032f);
        lightingpassShader.setFloat("pointLights[2].Radius", radius);

        lightingpassShader.setVec3("pointLights[3].ambient", {0.2f, 0.2f, 0.2f});
        lightingpassShader.setVec3("pointLights[3].diffuse", {0.5f, 0.5f, 0.5f});
        lightingpassShader.setVec3("pointLights[3].specular", {1.0f, 1.0f, 1.0f});
        lightingpassShader.setVec3("pointLights[3].position", lightPositions[3]);
        lightingpassShader.setFloat("pointLights[3].constant", 1.0f);
        lightingpassShader.setFloat("pointLights[3].linear", 0.09f);
        lightingpassShader.setFloat("pointLights[3].quadratic", 0.032f);
        lightingpassShader.setFloat("pointLights[3].Radius", radius);
        
        lightingpassShader.setInt("gPosition", 8);
        lightingpassShader.setInt("gNormal", 9);
        lightingpassShader.setInt("gAlbedoSpec", 10);
        lightingpassShader.setInt("ssao", 13);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        lightShader.use();
        lightShader.setVec3("LightColor", {1.0f, 1.0f, 1.0f});

        for (int i=0; i<4; i++) {
            lightShader.setMat4("model", lightModels[i]);
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glEnable(GL_BLEND);
        windowShader.use();
        windowShader.setMat4("view", view);
        windowShader.setMat4("projection", proj);
        windowShader.setInt("textureWindow", 2);

        std::map<float, Mat4> sortedPosition;
        for (int i=0; i<5; i++) {
            float distance = (cam.position - planePositions[i]).getMagnitude();
            sortedPosition[distance] = windowModels[i];
        }

        for (auto it=sortedPosition.rbegin(); it!=sortedPosition.rend(); it++) {
            windowShader.setMat4("model", it->second);
            glBindVertexArray(windowVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glDisable(GL_BLEND);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setMat4("projection", proj);
        skyboxShader.setMat4("view", viewSkybox);
        skyboxShader.setInt("skybox", 4);
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, t_skybox.getTextureID());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
        
        // glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。
        glfwSwapBuffers(window);
        // glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数
        glfwPollEvents();
    }
    // 释放/删除之前的分配的所有资源
    glfwTerminate();
}
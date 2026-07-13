cmake_minimum_required(VERSION 3.17)
project(OpenGLEngine)

set(CMAKE_CXX_STANDARD 17)

add_executable(OpenGLEngine
    app/src/main.cpp
    glad/src/glad.c
    shaders/src/Shader.cpp
    scene/src/Mesh.cpp
    scene/src/Model.cpp
    # scene/src/Texture.cpp
    scene/src/Camera.cpp
    mathtool/src/MathType.cpp
    external/stb_image/src/stb_image_impl.cpp
)

target_include_directories(OpenGLEngine PRIVATE
    include
    external/glfw/include
    external/stb_image/include
    external/assimp/include
    glad/include
)

target_link_directories(OpenGLEngine PRIVATE
    external/glfw/lib
    external/assimp/lib
)

target_compile_definitions(OpenGLEngine PRIVATE GLFW_STATIC)

target_link_libraries(OpenGLEngine PRIVATE
    glfw3
    opengl32
    assimp-vc143-mtd
)
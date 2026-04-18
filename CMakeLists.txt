cmake_minimum_required(VERSION 3.17)
project(OpenGLEngine)

set(CMAKE_CXX_STANDARD 17)

add_executable(OpenGLEngine
    app/src/main.cpp
    glad/src/glad.c
    shaders/src/Shader.cpp
    mesh/src/Mesh.cpp
    math/src/MathType.cpp
)

target_include_directories(OpenGLEngine PRIVATE
    include
    external/glfw/include
    glad/include
)

target_link_directories(OpenGLEngine PRIVATE
    external/glfw/lib
)

target_compile_definitions(OpenGLEngine PRIVATE GLFW_STATIC)

target_link_libraries(OpenGLEngine PRIVATE
    glfw3
    opengl32
)
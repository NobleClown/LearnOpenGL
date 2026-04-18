#pragma once
#include <vector>
#include <glad/glad.h>

class Mesh {
public:
    Mesh(const std::vector<float>& verticies);
    ~Mesh();

    void draw() const;
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int vertexCnt;
};
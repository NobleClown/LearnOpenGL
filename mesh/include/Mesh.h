#pragma once
#include <vector>
#include <glad/glad.h>

class Mesh {
public:
    Mesh(const std::vector<float>& verticies, const std::vector<unsigned int>& indices);
    ~Mesh();

    void draw() const;
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int vertexCnt;
    unsigned int indexCnt;
};
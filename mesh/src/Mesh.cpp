#include "../include/Mesh.h"

Mesh::Mesh(const std::vector<float>& verticies) {
    vertexCnt = verticies.size() / 3;

    // 创建VAO，用于解释Vertex数据
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 创建VBO，用于存储VertexBuffer数据
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 将cpu数据传到GPU
    glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(), GL_STATIC_DRAW);
    // 告诉GPU如何解释数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // 启用attribute
    glEnableVertexAttribArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCnt);
}
#include "../include/Mesh.h"

Mesh::Mesh(const std::vector<float>& verticies, const std::vector<unsigned int>& indices) {
    vertexCnt = verticies.size() / 3;
    indexCnt = indices.size() / 3;

    // 创建VAO，用于解释Vertex数据
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 创建VBO，用于存储VertexBuffer数据
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // 将cpu数据传到GPU
    glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(float), verticies.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
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
    // glDrawArrays(GL_TRIANGLES, 0, vertexCnt);
    glDrawElements(GL_TRIANGLES, indexCnt, GL_UNSIGNED_INT, 0);
}
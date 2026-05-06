#include "../include/Mesh.h"

void Mesh::uploadToGPU() {
    vertexCnt = m_verticies.size();
    indexCnt = m_indices.size();

    // 创建VAO，用于解释Vertex数据
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 创建VBO，用于存储VertexBuffer数据
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_verticies.size() * sizeof(Vertex), m_verticies.data(), GL_STATIC_DRAW);

    // 创建EBO，用于存储indexbuffer数据
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // 告诉GPU如何解释数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    // 解绑VAO，防止后续操作对其进行污染
    glBindVertexArray(0);
}

Mesh::Mesh(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indices) {
    m_verticies = verticies;
    m_indices = indices;
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
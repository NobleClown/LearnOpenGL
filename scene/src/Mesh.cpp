#include "../include/Mesh.h"

void Mesh::uploadToGPU() {
    // vertexCnt = m_verticies.size();
    // indexCnt = m_indices.size();

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
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    // 解绑VAO，防止后续操作对其进行污染
    glBindVertexArray(0);
}

Mesh::Mesh(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures) {
    m_verticies = verticies;
    m_indices = indices;
    m_textures = textures;

    uploadToGPU();
}

Mesh::~Mesh() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void Mesh::draw() const {
    glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, vertexCnt);
    glDrawElements(GL_TRIANGLES, indexCnt, GL_UNSIGNED_INT, 0);
}

// std::vector<Triangle> Mesh::getTriangles() const {
//     std::vector<Triangle> all_tri;

//     return all_tri;
// }

// AABB Mesh::getAABB() const {
//     AABB box;
//     for (const Vertex& v : m_verticies) {
//         box.x_max = std::max(box.x_max, v.position.x);
//         box.x_min = std::min(box.x_min, v.position.x);
//         box.y_max = std::max(box.y_max, v.position.y);
//         box.y_min = std::min(box.y_min, v.position.y);
//         box.z_max = std::max(box.z_max, v.position.z);
//         box.z_min = std::min(box.z_min, v.position.z);
//     }
//     return box;
// }
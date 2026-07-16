#include "../include/Mesh.h"

void Mesh::uploadToGPU() {

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
    // if (VAO != 0) {
    //     glDeleteVertexArrays(1, &VAO);
    //     glDeleteBuffers(1, &VBO);
    // }
}

void Mesh::Draw(const Shader& shader) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i=0; i<m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = m_textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);
        shader.setInt("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::DrawInstances(const Shader& shader, unsigned int amount) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i=0; i<m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = m_textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);
        shader.setInt("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);
}

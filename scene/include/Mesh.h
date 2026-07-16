#pragma once
#include <vector>
#include <glad/glad.h>

#include "Material.h"
#include "Texture.h"
#include "../../mathtool/include/MathType.h"
#include "../../shaders/include/Shader.h"

class Mesh {
public:
    Mesh() {}
    Mesh(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
    void uploadToGPU();
    std::vector<Vertex> m_verticies;
    std::vector<uint32_t> m_indices;
    std::vector<Texture> m_textures;
    void Draw(const Shader& shader);
    void DrawInstances(const Shader& shader, unsigned int amount);
    unsigned int getVAO() { return VAO; }
    ~Mesh();

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
};

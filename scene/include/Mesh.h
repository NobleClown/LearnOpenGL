#pragma once
#include <vector>
#include <glad/glad.h>

#include "Material.h"
#include "../../mathtool/include/MathType.h"

class Mesh {
public:
    Mesh() {}
    Mesh(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indices);
    void uploadToGPU();
    std::string name;
    std::vector<Vertex> getVerticies() const { return m_verticies; }
    std::vector<uint32_t> getIndexBuffer() const { return m_indices; }
    void addVertex(const Vertex& v) { m_verticies.emplace_back(v); }
    void addIndex(const uint32_t& p) { m_indices.emplace_back(p); }
    void setMaterial(Material* _material) { m_material = _material; }
    Material* getMaterial() const { return m_material; }
    ~Mesh();

    void draw() const;
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int vertexCnt;
    unsigned int indexCnt;

    std::vector<Vertex> m_verticies;
    std::vector<uint32_t> m_indices;
    Material* m_material;
};

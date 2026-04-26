#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "../../mathtool/include/MathType.h"
#include "Material.h"

struct Triangle {
    uint32_t idx0, idx1, idx2;
};

struct VertexKey {
    int vIdx;   // 位置索引
    int vtIdx;  // UV 索引
    int vnIdx;  // 法线索引

    VertexKey() = default;
    VertexKey(int vidx, int vtidx, int vnidx) : vIdx(vidx), vtIdx(vtidx), vnIdx(vnidx) {}

    bool operator==(const VertexKey& other) const {
        return vIdx == other.vIdx && vtIdx == other.vtIdx && vnIdx == other.vnIdx;
    }
};

struct VertexKeyHash {
    size_t operator()(const VertexKey& key) const {
        size_t h1 = std::hash<int>{}(key.vIdx);
        size_t h2 = std::hash<int>{}(key.vtIdx);
        size_t h3 = std::hash<int>{}(key.vnIdx);
        // 用一个简单的组合方式把三个哈希值合成一个
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

class Mesh {
public:
    std::string name;
    std::vector<Vertex> getVerticies() const { return verticies; }
    std::vector<uint32_t> getIndexBuffer() const { return indices; }
    void addVertex(const Vertex& v) { verticies.emplace_back(v); }
    void addIndex(const uint32_t& p) { indices.emplace_back(p); }
    void setMaterial(Material* _material) { material = _material; }
    Material* getMaterial() const { return material; }
private:
    std::vector<Vertex> verticies;
    std::vector<uint32_t> indices;
    Material* material;
};

class Model {
public:
    bool LoadOBJ(const std::string& path);
    std::vector<Mesh> GetMeshes() const { return m_meshes; }
    bool setMaterial(int idx, Material* _material) {
        if (idx > m_meshes.size())
            return false;
        m_meshes[idx].setMaterial(_material);
        return true;
    }
private:
    // 用于处理obj文件中坐标从1计数和负索引的问题
    inline int fixIndex(int idx, int size) {
        if (idx > 0)
            return idx - 1;
        else
            return idx + size;
    }
    std::vector<Mesh> m_meshes;
};
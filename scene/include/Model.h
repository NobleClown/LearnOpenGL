#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "../../mathtool/include/MathType.h"
#include "Mesh.h"
#include "Material.h"
#include "../../external/assimp/include/assimp/Importer.hpp"
#include "../../external/assimp/include/assimp/scene.h"
#include "../../external/assimp/include/assimp/postprocess.h"


class Model {
public:
    Model(const std::string& path);
    void Draw(const Shader& shader);
    void DrawInstances(const Shader& shader, unsigned int amount);
    std::vector<Mesh>& getMeshse() { return m_meshes; }
private:
    bool loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMatierialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const std::string& path, const std::string& directory, bool gamma);
    
    std::vector<Mesh> m_meshes;
    std::string m_directory;
    std::vector<Texture> textures_loaded;
};
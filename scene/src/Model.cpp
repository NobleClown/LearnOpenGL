#include <unordered_map>
#include "../include/Model.h"

Model::Model(const std::string& path) {
    loadModel(path);
}

bool Model::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "[ERROR] [Loading Model] [ASSIMP]" << importer.GetErrorString() << std::endl;
        return false;
    }

    m_directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);

    return true;
}

void Model::Draw(const Shader& shader) {
    for (Mesh mesh : m_meshes) 
        mesh.Draw(shader);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i=0; i<node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh tmpMesh = processMesh(mesh, scene);
        m_meshes.push_back(tmpMesh);
    }

    for (unsigned int i=0; i<node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // 复制顶点数据
    for (unsigned int i=0; i<mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };
        vertex.normal = {
            mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z
        };
        if (mesh->mTextureCoords[0]) {
            vertex.uv = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        } else
            vertex.uv = {0.0, 0.0};
        
        vertices.push_back(vertex);
    }

    // 复制索引数据
    for (unsigned int i=0; i<mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j=0; j<face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMatierialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMatierialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMatierialTextures(aiMaterial* material, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;

    for (unsigned int i=0; i<material->GetTextureCount(type); i++) {
        aiString path;
        material->GetTexture(type, i, &path);
        bool skip = false;
        for (unsigned int j=0; j<textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.C_Str(), path.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) {
            Texture texture;
            texture.id = TextureFromFile(path.C_Str(), m_directory, false);
            texture.type = typeName;
            texture.path = path;
            textures.push_back(texture);
        }
    }
    return textures;
}

unsigned int Model::TextureFromFile(const std::string& path, const std::string& directory, bool gamma) {
    std::string filename = directory + "/" + path;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "failed to load texture with path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
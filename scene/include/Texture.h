#pragma once

#include <string>
#include <vector>

#include "../../external/stb_image/include/stb_image.h"

class Texture {
public:
    void loadTexture(const char* path, const int texCnt);
    void loadTexture(const int texCnt);
    void setTexture();
    unsigned int getTextureID() const { return texture; }
private:
    int m_width;
    int m_height;
    int m_nrChannels;
    unsigned char* data = nullptr;
    unsigned int texture;
    std::string type;
};

class CubeTexture {
public:
    void loadTexture(const std::vector<std::string>& paths, const int texCnt);
    void setTexture();
    unsigned int getTextureID() const { return texture; }
private:
    int m_width;
    int m_height;
    int m_nrChannels;
    unsigned int texture;
};
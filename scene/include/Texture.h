#pragma once

#include <string>

#include "../../external/stb_image/include/stb_image.h"

class Texture {
public:
    void loadTexture(const char* path, const int texCnt);
    void setTexture();
private:
    int m_width;
    int m_height;
    int m_nrChannels;
    unsigned char* data = nullptr;
    unsigned int texture;
    std::string type;
};
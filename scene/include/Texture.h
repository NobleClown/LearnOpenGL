#pragma once

#include <string>

#include "../../external/stb_image/include/stb_image.h"
#include "../../external/assimp/include/assimp/Importer.hpp"

class Texture {
public:
    unsigned int id;
    std::string type;
    aiString path;
};
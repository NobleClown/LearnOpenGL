#pragma once
#include "../../mathtool/include/MathType.h"
// #include "Texture.h"

struct Material {
    // Texture2D* diffuseMap;

    Vec3 specularColor = Vec3(0.5f, 0.f, 0.f);
    float shininess = 32.0f;
};
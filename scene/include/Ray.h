#pragma once

#include "../../mathtool/include/MathType.h"
#include "Mesh.h"

struct Ray {
    Vec3 position;
    Vec3 direction;
    bool hitMesh(const Mesh& mesh, Vec3& posi) const;
    bool hasInteract(const AABB& box) const;
    // bool isInTriangle(const Triangle& triangle, Vec3& posi) const;
};
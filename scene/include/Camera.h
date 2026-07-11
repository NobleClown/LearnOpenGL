#pragma once
#include "../../mathtool/include/MathType.h"
#include "Ray.h"

class Camera {
public:
    Vec3 position;
    Vec3 forward;
    Vec3 up;

    float fov;
    float aspect;
    float nearPlane;
    float farPlane;
    float speed;

    Mat4 getViewMat() const;
    Mat4 getProjectionMat() const;
    Mat4 getVPMatrix() const;

    Ray generateRay(const Vec2& p, const Vec2& resolution) const;
};
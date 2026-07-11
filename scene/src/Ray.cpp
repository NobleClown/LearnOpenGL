#include "../include/Ray.h"
#include <algorithm>
#include <cmath>
#include <limits>

// ==================== hasInteract ====================
bool Ray::hasInteract(const AABB& box) const {
    float t_min = -std::numeric_limits<float>::infinity();
    float t_max = std::numeric_limits<float>::infinity();
    
    // X轴
    if (std::abs(direction.x) < 1e-8f) {
        if (position.x < box.x_min || position.x > box.x_max)
            return false;
    } else {
        float t1 = (box.x_min - position.x) / direction.x;
        float t2 = (box.x_max - position.x) / direction.x;
        t_min = std::max(t_min, std::min(t1, t2));
        t_max = std::min(t_max, std::max(t1, t2));
        if (t_max < t_min) return false;
        if (t_max < 0) return false;
    }
    
    // Y轴
    if (std::abs(direction.y) < 1e-8f) {
        if (position.y < box.y_min || position.y > box.y_max)
            return false;
    } else {
        float t3 = (box.y_min - position.y) / direction.y;
        float t4 = (box.y_max - position.y) / direction.y;
        t_min = std::max(t_min, std::min(t3, t4));
        t_max = std::min(t_max, std::max(t3, t4));
        if (t_max < t_min) return false;
        if (t_max < 0) return false;
    }
    
    // Z轴
    if (std::abs(direction.z) < 1e-8f) {
        if (position.z < box.z_min || position.z > box.z_max)
            return false;
    } else {
        float t5 = (box.z_min - position.z) / direction.z;
        float t6 = (box.z_max - position.z) / direction.z;
        t_min = std::max(t_min, std::min(t5, t6));
        t_max = std::min(t_max, std::max(t5, t6));
        if (t_max < t_min) return false;
        if (t_max < 0) return false;
    }
    
    return t_min >= 0;
}

// ==================== isInTriangle ====================
bool Ray::isInTriangle(const Triangle& triangle, Vec3& posi) const {
    const float EPSILON = 1e-8f;
    
    Vec3 edge1 = triangle.v1.position - triangle.v0.position;
    Vec3 edge2 = triangle.v2.position - triangle.v0.position;
    Vec3 pvec = direction.crossProduct(edge2);
    
    float det = edge1.dotProduct(pvec);
    
    // 光线与三角形平面平行
    if (det > -EPSILON && det < EPSILON)
        return false;
    
    float invDet = 1.0f / det;
    Vec3 tvec = position - triangle.v0.position;
    
    float u = tvec.dotProduct(pvec) * invDet;
    if (u < 0.0f || u > 1.0f)
        return false;
    
    Vec3 qvec = tvec.crossProduct(edge1);
    float v = direction.dotProduct(qvec) * invDet;
    if (v < 0.0f || (u + v) > 1.0f)
        return false;
    
    float t = edge2.dotProduct(qvec) * invDet;
    if (t < 0.0f)
        return false;
    
    posi = position + direction * t;
    return true;
}

// ==================== hitMesh ====================
bool Ray::hitMesh(const Mesh& mesh, Vec3& posi) const {
    // AABB快速剔除
    AABB box = mesh.getAABB();
    if (!hasInteract(box))
        return false;
    
    // 获取所有三角形（注意：如果getTriangles返回引用，需确保Mesh内部存储持久）
    std::vector<Triangle> triangles = mesh.getTriangles();
    
    Vec3 closestPosi;
    float closestT = std::numeric_limits<float>::infinity();
    bool hitFound = false;
    
    for (const Triangle& tri : triangles) {
        Vec3 intersectPosi;
        if (isInTriangle(tri, intersectPosi)) {
            // 计算距离（使用距离平方避免开方，但为了比较t值，直接计算t更准确）
            Vec3 diff = intersectPosi - position;
            float t = diff.getMagnitude(); // 或者 diff.dotProduct(diff) 用于平方比较
            
            if (t < closestT) {
                closestT = t;
                closestPosi = intersectPosi;
                hitFound = true;
            }
        }
    }
    
    if (hitFound) {
        posi = closestPosi;
        return true;
    }
    return false;
}
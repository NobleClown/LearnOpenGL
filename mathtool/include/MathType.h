#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

constexpr float PI = 3.14159265358979f;

struct Vec2 {
    float x, y;

    Vec2 operator+(const Vec2& other) const { return Vec2{x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return Vec2{x - other.x, y - other.y}; }
    Vec2 operator*(float f) const { return Vec2{x * f, y * f}; }
};

struct Vec3 {
    float x, y, z;

    Vec3() {}
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Vec3 operator-(const Vec3& other) const { return Vec3{x - other.x, y - other.y, z - other.z}; }
    Vec3 operator+(const Vec3& other) const { return Vec3{x + other.x, y + other.y, z + other.z}; }
    Vec3 operator/(const float div) const { return Vec3{x / div, y / div, z / div}; }
    Vec3 operator*(const float m) const { return Vec3{x * m, y * m, z * m}; }
    Vec3 crossProduct(const Vec3& other) const { return Vec3{y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x}; }
    float dotProduct(const Vec3& other) const { return x * other.x + y * other.y + z * other.z; }
    float getMagnitude() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3 normalize() const {
        float mag = getMagnitude();
        if (mag < 1e-6)
            return Vec3{0.f, 0.f, 0.f};
        return Vec3{x / mag, y / mag, z / mag};
    }
};

struct Vec4 {
    float x, y, z, w;
    Vec4() {}
    Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vec4(const Vec3& v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) {}
    Vec4 operator*(const float m) const { return Vec4{ x * m, y * m, z * m, w * m }; }
    Vec4 operator/(const float m) const { return Vec4{ x / m, y / m, z / m, w / m }; }
};

struct Mat4 {
    float m[16] = {0};

    static Mat4 identity() {
        Mat4 r;
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }

    static Mat4 getTranslateMat(const Vec3& t) {
        Mat4 transMat = Mat4::identity();
        transMat(0, 3) = t.x;
        transMat(1, 3) = t.y;
        transMat(2, 3) = t.z;
        return transMat;
    }

    static Mat4 getScaleMat(const Vec3& s) {
        Mat4 scaleMat;
        scaleMat(0, 0) = s.x;
        scaleMat(1, 1) = s.y;
        scaleMat(2, 2) = s.z;
        scaleMat(3, 3) = 1;
        return scaleMat;
    }

    static Mat4 getRotateMat(const Vec3& euler) {
        Mat4 rotateX;
        Mat4 rotateY;
        Mat4 rotateZ;

        // rotate with x
        Vec3 rad = euler * PI / 180.f;
        float sin_x = std::sinf(rad.x);
        float cos_x = std::cosf(rad.x);
        rotateX(0, 0) = 1;
        rotateX(1, 1) = cos_x;
        rotateX(2, 2) = cos_x;
        rotateX(1, 2) = -sin_x;
        rotateX(2, 1) = sin_x;
        rotateX(3, 3) = 1;

        // rotate with y 因为 y = z \times x, 因此绕y轴的旋转角度为顺时针，即\theta为负数
        float sin_y = std::sinf(rad.y);
        float cos_y = std::cosf(rad.y);
        rotateY(0, 0) = cos_y;
        rotateY(1, 1) = 1;
        rotateY(2, 2) = cos_y;
        rotateY(0, 2) = sin_y;
        rotateY(2, 0) = -sin_y;
        rotateY(3, 3) = 1;
        
        float sin_z = std::sinf(rad.z);
        float cos_z = std::cosf(rad.z);
        rotateZ(0, 0) = cos_z;
        rotateZ(1, 1) = cos_z;
        rotateZ(2, 2) = 1;
        rotateZ(0, 1) = -sin_z;
        rotateZ(1, 0) = sin_z;
        rotateZ(3, 3) = 1;

        return rotateZ * rotateY * rotateX;
    }

    static Mat4 getPerspectiveMat(float fov, float aspect, float nearP, float farP) {
        Mat4 perspMat;
        float t = tanf(fov / 2.0f * PI / 180.f);
        perspMat(0, 0) = 1.0f / (aspect * t);
        perspMat(1, 1) = 1.0f / t;
        perspMat(2, 2) = -(nearP + farP) / (farP - nearP);
        perspMat(2, 3) = - 2 * nearP * farP / (farP - nearP);
        perspMat(3, 2) = -1;
        return perspMat;
    }

    static Mat4 getViewMat(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = center - eye;
        f = f.normalize();
        Vec3 right = f.crossProduct(up);
        right = right.normalize();
        Vec3 u = right.crossProduct(f);
        u = u.normalize();
        Mat4 viewMat;
        viewMat(0, 0) = right.x;
        viewMat(0, 1) = right.y;
        viewMat(0, 2) = right.z;
        viewMat(0, 3) = -right.dotProduct(eye);
        viewMat(1, 0) = u.x;
        viewMat(1, 1) = u.y;
        viewMat(1, 2) = u.z;
        viewMat(1, 3) = -u.dotProduct(eye);
        viewMat(2, 0) = -f.x;
        viewMat(2, 1) = -f.y;
        viewMat(2, 2) = -f.z;
        viewMat(2, 3) = f.dotProduct(eye);
        viewMat(3, 3) = 1;
        return viewMat;
    }

    inline float get(int row, int col) const {
        assert(row >= 0 && row < 4 && col >= 0 && col < 4);
        return m[col * 4 + row];
    }

    inline void set(int row, int col, float value) {
        assert(row >= 0 && row < 4 && col >= 0 && col < 4);
        m[col * 4 + row] = value;
        return;
    }

    // 运算
    Mat4 operator*(const Mat4& other) const {
        Mat4 res;
        for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++) {
                float sum = 0.f;
                for (int k=0; k<4; k++) {
                    sum += get(i, k) * other.get(k, j);
                }
                res.set(i, j, sum);
            }
        }
        return res;
    }

    Vec4 operator*(const Vec4& other) const {
        return Vec4{
            m[0] * other.x + m[4] * other.y + m[8]  * other.z + m[12] * other.w,
            m[1] * other.x + m[5] * other.y + m[9]  * other.z + m[13] * other.w,
            m[2] * other.x + m[6] * other.y + m[10] * other.z + m[14] * other.w,
            m[3] * other.x + m[7] * other.y + m[11] * other.z + m[15] * other.w
        };
    }

    inline float& operator()(int row, int col) {
        assert(row >= 0 && row < 4 && col >= 0 && col < 4);
        return m[col * 4 + row];
    }
    
};
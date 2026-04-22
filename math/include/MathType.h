#pragma once
#include <cmath>
#include <iostream>

constexpr double PI = 3.14159265358979323846;

struct Vec2 {
    float x, y;

    Vec2 operator+(const Vec2& other) const {
        Vec2 res;
        res.x = x + other.x;
        res.y = y + other.y;
        return res;
    }
    Vec2 operator-(const Vec2& other) const {
        Vec2 res;
        res.x = x - other.x;
        res.y = y - other.y;
        return res;
    }
    Vec2 operator*(float f) const {
        Vec2 res;
        res.x = x * f;
        res.y = y * f;
        return res;
    }
};

struct Vec3 {
    float x, y, z;

    Vec3() {}
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    Vec3 operator-(const Vec3& other) const {
        Vec3 res;
        res.x = x - other.x;
        res.y = y - other.y;
        res.z = z - other.z;
        return res;
    }

    Vec3 operator+(const Vec3& other) const {
        Vec3 res;
        res.x = x + other.x;
        res.y = y + other.y;
        res.z = z + other.z;
        return res;
    }

    Vec3 operator/(const float div) const {
        Vec3 res;
        res.x = x / div;
        res.y = y / div;
        res.z = z / div;
        return res;
    }

    Vec3 operator*(const float m) const {
        Vec3 res;
        res.x = x * m;
        res.y = y * m;
        res.z = z * m;
        return res;
    }

    Vec3 crossProduct(const Vec3& other) const {
        Vec3 res;
        res.x = y * other.z - z * other.y;
        res.y = z * other.x - x * other.z;
        res.z = x * other.y - y * other.x;
        return res;
    }

    float dotProduct(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    float getMagnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3 normalize() const {
        float mat = getMagnitude();
        Vec3 res;
        res.x = x / mat;
        res.y = y / mat;
        res.z = z / mat;
        return res;
    }
};

struct Vec4 {
    float x, y, z, w;

    Vec4() {}
    Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vec4(const Vec3& v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) {}
    Vec4 operator*(const float m) const {
        Vec4 res;
        res.x = x * m;
        res.y = y * m;
        res.z = z * m;
        res.w = w * m;
        return res;
    }

    Vec4 operator/(const float m) const {
        Vec4 res;
        res.x = x / m;
        res.y = y / m;
        res.z = z / m;
        res.w = w / m;
        return res;
    }
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
        transMat.set(0, 3, t.x);
        transMat.set(1, 3, t.y);
        transMat.set(2, 3, t.z);
        return transMat;
    }

    static Mat4 getScaleMat(const Vec3& s) {
        Mat4 scaleMat;
        scaleMat.set(0, 0, s.x);
        scaleMat.set(1, 1, s.y);
        scaleMat.set(2, 2, s.z);
        scaleMat.set(3, 3, 1);
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
        rotateX.set(0, 0, 1);
        rotateX.set(1, 1, cos_x);
        rotateX.set(2, 2, cos_x);
        rotateX.set(1, 2, -sin_x);
        rotateX.set(2, 1, sin_x);
        rotateX.set(3, 3, 1);

        // rotate with y 因为 y = z \times x, 因此绕y轴的旋转角度为顺时针，即\theta为负数
        float sin_y = std::sinf(rad.y);
        float cos_y = std::cosf(rad.y);
        rotateY.set(0, 0, cos_y);
        rotateY.set(1, 1, 1);
        rotateY.set(2, 2, cos_y);
        rotateY.set(0, 2, sin_y);
        rotateY.set(2, 0, -sin_y);
        rotateY.set(3, 3, 1);
        
        float sin_z = std::sinf(rad.z);
        float cos_z = std::cosf(rad.z);
        rotateZ.set(0, 0, cos_z);
        rotateZ.set(1, 1, cos_z);
        rotateZ.set(2, 2, 1);
        rotateZ.set(0, 1, -sin_x);
        rotateZ.set(1, 0, sin_x);
        rotateZ.set(3, 3, 1);

        return rotateX * rotateY * rotateZ;
    }

    static Mat4 getPerspectiveMat(float fov, float aspect, float nearP, float farP) {
        Mat4 perspMat;
        float t = tan(fov / 2.0f * PI / 180.f);
        perspMat.set(0, 0, 1.0f / (aspect * t));
        perspMat.set(1, 1, 1.0f / t);
        perspMat.set(2, 2, -(nearP + farP) / (farP - nearP));
        perspMat.set(2, 3, - 2 * nearP * farP / (farP - nearP));
        perspMat.set(3, 2, -1);
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
        viewMat.set(0, 0, right.x);
        viewMat.set(0, 1, right.y);
        viewMat.set(0, 2, right.z);
        viewMat.set(0, 3, -right.dotProduct(eye));
        viewMat.set(1, 0, u.x);
        viewMat.set(1, 1, u.y);
        viewMat.set(1, 2, u.z);
        viewMat.set(1, 3, -u.dotProduct(eye));
        viewMat.set(2, 0, -f.x);
        viewMat.set(2, 1, -f.y);
        viewMat.set(2, 2, -f.z);
        viewMat.set(2, 3, f.dotProduct(eye));
        viewMat.set(3, 3, 1);
        return viewMat;
    }

    inline float get(int row, int col) const {
        if (row >= 4 || col >= 4) {
            std::cout << "[ERROR] row or col is overflow" << std::endl; 
            return -1;
        }
        return m[col * 4 + row];
    }

    inline void set(int row, int col, float value) {
        if (row >= 4 || col >= 4) {
            std::cout << "[ERROR] row or col is overflow" << std::endl; 
            return;
        }
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
        Vec4 res;
        for (int i=0; i<4; i++) {
            res.x = get(0, 0) * other.x + get(0, 1) * other.y + get(0, 2) * other.z + get(0, 3) * other.w;
            res.y = get(1, 0) * other.x + get(1, 1) * other.y + get(1, 2) * other.z + get(1, 3) * other.w;
            res.z = get(2, 0) * other.x + get(2, 1) * other.y + get(2, 2) * other.z + get(2, 3) * other.w;
            res.w = get(3, 0) * other.x + get(3, 1) * other.y + get(3, 2) * other.z + get(3, 3) * other.w;
        }
        return res;
    }

    
};
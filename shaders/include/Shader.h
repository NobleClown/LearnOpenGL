#pragma once
#include <string>
#include <unordered_map>
#include <glad/glad.h>

#include "../../math/include/MathType.h"

class Shader {
public:
    Shader(const std::string& vsPath, const std::string& fsPath);
    ~Shader();

    void use() const;

    unsigned int getProgramID() const { return programID; }

    // for uniform
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setMat4(const std::string& name, const Mat4& mat);
private:
    unsigned int programID;

    std::string loadFile(const std::string& path);
    unsigned int compile(GLenum type, const std::string& src);
    void link(unsigned int vs, unsigned int fs);
};

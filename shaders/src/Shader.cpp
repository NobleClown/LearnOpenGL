#include "../include/Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vsPath, const std::string& fsPath) {
    std::string vsCode = loadFile(vsPath);
    std::string fsCode = loadFile(fsPath);

    unsigned int vs = compile(GL_VERTEX_SHADER, vsCode);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fsCode);

    link(vs, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::Shader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath) {
    std::string vsCode = loadFile(vsPath);
    std::string fsCode = loadFile(fsPath);
    std::string gsCode = loadFile(gsPath);

    unsigned int vs = compile(GL_VERTEX_SHADER, vsCode);
    unsigned int fs = compile(GL_FRAGMENT_SHADER, fsCode);
    unsigned int gs = compile(GL_GEOMETRY_SHADER, gsCode);

    link(vs, fs, gs);

    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteShader(gs);
}

Shader::~Shader() {
    glDeleteProgram(programID);
}

std::string Shader::loadFile(const std::string& path) {
    std::fstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::compile(GLenum type, const std::string& src) {
    unsigned int shader = glCreateShader(type);
    const char* csrc = src.c_str();
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cout << "Shader compile error: \n " << log << std::endl;
    }

    return shader;
}

void Shader::link(unsigned int vs, unsigned int fs) {

    programID = glCreateProgram();
    glAttachShader(programID, vs);
    glAttachShader(programID, fs);

    glLinkProgram(programID);

    checkLinkStatus();
}

void Shader::link(unsigned int vs, unsigned int fs, unsigned int gs) {
    programID = glCreateProgram();
    glAttachShader(programID, vs);
    glAttachShader(programID, fs);
    glAttachShader(programID, gs);

    glLinkProgram(programID);

    checkLinkStatus();
}

void Shader::checkLinkStatus() const {
    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(programID, 512, nullptr, log);
        std::cout << "Program Link Error: \n" << log << std::endl;
    }
}

void Shader::use() const {
    glUseProgram(programID);
}

void Shader::setMat4(const std::string& name, const Mat4& mat) {
    int loc = glGetUniformLocation(programID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat.m);
}

void Shader::setVec3(const std::string& name, const Vec3& vec) {
    int loc = glGetUniformLocation(programID, name.c_str());
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

void Shader::setInt(const std::string& name, const int value) {
    int loc = glGetUniformLocation(programID, name.c_str());
    glUniform1i(loc, value);
}

void Shader::setFloat(const std::string& name, const float value) {
    int loc = glGetUniformLocation(programID, name.c_str());
    glUniform1f(loc, value);
}
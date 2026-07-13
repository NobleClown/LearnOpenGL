#include "../include/Texture.h"
#include <glad/glad.h>
#include <glfw3.h>

void Texture::loadTexture(const char* path, const int texCnt) {
    // 加载纹理图像
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(path, &m_width, &m_height, &m_nrChannels, 0);
    // 生成纹理（空间）
    glGenTextures(1, &texture);
    // 激活纹理
    glActiveTexture(GL_TEXTURE0 + texCnt);
    // 绑定纹理（当前处理该纹理）
    glBindTexture(GL_TEXTURE_2D, texture);
    // 设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 使用data中的图像，填充纹理空间
    if (m_nrChannels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if (m_nrChannels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // 根据当前纹理，生成mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    // glBindTexture(GL_TEXTURE_2D, 0);
    //释放内存
    stbi_image_free(data);  
}

void Texture::setTexture() {
    glBindTexture(GL_TEXTURE_2D, texture);
}
#version 330 core

layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 uv;
layout (location = 1) in vec3 norm;
// layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

out vec3 FragPos;
out vec3 FragNormal;

// out vec3 ourColor;
out vec2 TexCoord;
out vec3 Norm;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    FragNormal = mat3(transpose(inverse(model))) * norm;

    gl_Position = projection * view * worldPos;
    // gl_Position = model * vec4(aPos, 1.0);
    // ourColor = aColor;
    TexCoord = aTexCoord;
}
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 norm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 FragNormal;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    FragNormal = mat3(transpose(inverse(model))) * norm;

    gl_Position = projection * view * worldPos;
}
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 FragNormal;
} vs_out;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;
    
    vs_out.FragNormal = mat3(transpose(inverse(model))) * norm;

    gl_Position = projection * view * worldPos;
    vs_out.TexCoords = aTexCoord;
}
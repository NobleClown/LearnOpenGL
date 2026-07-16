#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceModel;

// uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 FragNormal;
} vs_out;

void main() {
    vec4 worldPos = instanceModel * vec4(aPos, 1.0);
    vs_out.FragPos = worldPos.xyz;
    
    vs_out.FragNormal = mat3(transpose(inverse(instanceModel))) * norm;

    gl_Position = projection * view * worldPos;
    vs_out.TexCoords = aTexCoord;
}
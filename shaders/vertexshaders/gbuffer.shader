#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangentVec;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

out vec3 FragPos;
out vec3 FragNormal;
out vec2 TexCoords;
out mat3 TBN;

void main() {
    vec4 worldPos = view * model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    FragNormal = mat3(transpose(inverse(view * model))) * norm;

    gl_Position = projection * worldPos;
    TexCoords = aTexCoord;

    vec3 T = normalize(tangentVec);
    vec3 N = vec3(0.0, 0.0, 1.0);
    T = normalize(T - N * dot(T, N));
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);
}
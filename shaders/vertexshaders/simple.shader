#version 330 core

layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 uv;
layout (location = 1) in vec3 norm;
// layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (location = 3) in vec3 tangentVec;

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform vec3 cameraPos;
// uniform mat4 view;
// uniform mat4 projection;

out vec3 FragPos;
out vec3 FragNormal;

// out vec3 ourColor;
out vec2 TexCoord;
out vec3 Norm;
out mat3 TBN;
out vec3 viewDir;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    FragNormal = mat3(transpose(inverse(model))) * norm;

    gl_Position = projection * view * worldPos;
    // gl_Position = model * vec4(aPos, 1.0);
    // ourColor = aColor;

    vec3 T = normalize(tangentVec);
    vec3 N = vec3(0.0, 0.0, 1.0);
    T = normalize(T - N * dot(T, N));
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);
    viewDir = normalize(TBN * (cameraPos - FragPos));
    TexCoord = aTexCoord;
}
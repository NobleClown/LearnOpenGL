#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 FragNormal;
} gs_in[];

out vec2 TexCoord;
out vec3 FragPos;
out vec3 FragNormal;

uniform float time;

vec3 GetNormal() {
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 1.0);
}

void main() {
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoord = gs_in[0].TexCoords;
    FragPos = gs_in[0].FragPos;
    FragNormal = gs_in[0].FragNormal;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoord = gs_in[1].TexCoords;
    FragPos = gs_in[1].FragPos;
    FragNormal = gs_in[1].FragNormal;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoord = gs_in[2].TexCoords;
    FragPos = gs_in[2].FragPos;
    FragNormal = gs_in[2].FragNormal;
    EmitVertex();
    EndPrimitive();
}
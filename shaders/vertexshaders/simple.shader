#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out VS_OUT {
    vec3 color;
} vs_out;

uniform mat4 model;

void main() {
    vec2 pos = aPos;
    gl_Position = model * vec4(pos, 0.0, 1.0);
    vs_out.color = aColor;
}
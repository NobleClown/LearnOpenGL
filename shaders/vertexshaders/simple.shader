#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out VS_OUT {
    vec3 color;
} vs_out;

uniform vec2 offsets[100];

void main() {
    vec2 pos = aPos * (gl_InstanceID / 100.0);
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(pos + offset, 0.0, 1.0);
    vs_out.color = aColor;
}
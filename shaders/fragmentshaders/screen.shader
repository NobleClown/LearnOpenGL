#version 330 core

const float offset = 1.0 / 300.0;

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D textureSceen;

void main() {
    vec2 offsets[] = vec2[](
        vec2(-offset, offset),
        vec2(-offset, 0),
        vec2(-offset, -offset),
        vec2(0, -offset),
        vec2(offset, -offset),
        vec2(offset, 0),
        vec2(offset, offset),
        vec2(0, offset)
    );

    vec3 texColor = vec3(texture(textureSceen, TexCoord)) * 4;

    for (int i=0; i<8; i++) {
        texColor += 1 * vec3(texture(textureSceen, TexCoord + offsets[i]));
    }
    FragColor = vec4(texColor / 12, 1.0);
}
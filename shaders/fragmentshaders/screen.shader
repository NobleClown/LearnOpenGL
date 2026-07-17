#version 330 core

in vec2 TexCoords;

uniform sampler2D screenTexture;

out vec4 FragColor;

void main() {
    FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0);
}


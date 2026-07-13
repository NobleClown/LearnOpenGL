#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D textureWindow;

void main() {
    vec4 texColor = texture(textureWindow, TexCoord);

    FragColor = texColor;
}
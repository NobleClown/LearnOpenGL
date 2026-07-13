#version 330 core

in vec2 TexCoord;

uniform sampler2D textureGrass;

out vec4 FragColor;

void main() {
    FragColor = texture(textureGrass, TexCoord);
}
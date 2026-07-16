#version 330 core

in VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 FragNormal;
} fs_in;

uniform sampler2D textureRock;

out vec4 FragColor;

void main() {
    FragColor = texture(textureRock, fs_in.TexCoords);
}
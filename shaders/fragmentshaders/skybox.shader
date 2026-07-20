#version 330 core

in vec3 TexCoords;

uniform samplerCube skybox;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main() {
    FragColor = texture(skybox, TexCoords);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness >= 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
}
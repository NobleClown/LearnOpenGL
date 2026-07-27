#version 330 core

in vec3 TexCoords;

uniform samplerCube skybox;
out vec4 FragColor;

void main() {
    vec3 envColor = texture(skybox, TexCoords).rgb;
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0 / 2.2));

    FragColor = vec4(envColor, 1.0);
}
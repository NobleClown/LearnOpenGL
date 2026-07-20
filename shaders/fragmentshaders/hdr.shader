#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform sampler2D blurBloom;
uniform float exposure;

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    vec3 bloomColor = texture(blurBloom, TexCoords).rgb;
    hdrColor += bloomColor;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}
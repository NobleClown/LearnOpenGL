#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gMetalRough;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 FragNormal;
in mat3 TBN;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;
uniform float nearPlane;
uniform float farPlane;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2 * nearPlane * farPlane) / (nearPlane + farPlane - z * (farPlane - nearPlane));
}

void main() {
    gPosition = vec4(FragPos, LinearizeDepth(gl_FragCoord.z));
    // gNormal = vec4(TBN * texture(texture_normal, TexCoords).xyz, 0.0);
    gNormal = vec4(FragNormal, 1.0);

    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;
    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;
    gMetalRough.r = texture(texture_metallic, TexCoords).r;
    gMetalRough.g = texture(texture_roughness, TexCoords).r;
}
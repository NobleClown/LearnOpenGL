#version 330 core

in vec3 TexCoords;

uniform samplerCube loft;

out vec4 FragColor;

const float pi = 3.14159265359;

void main() {
    vec3 normal = normalize(TexCoords);
    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    float sampleData = 0.025;
    float nrSamples = 0.0;
    
    for (float phi=0.0; phi<2.0*pi; phi+=sampleData) {
        for (float theta=0.0; theta<0.5*pi; theta+=sampleData) {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
            irradiance += texture(loft, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = pi * irradiance * (1.0 / nrSamples);
    FragColor = vec4(irradiance, 1.0);
}
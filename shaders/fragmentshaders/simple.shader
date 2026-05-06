#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 FragNormal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform vec3 cameraPos;

void main() {
    // 基础向量
    vec3 norm = normalize(FragNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 halfDir = normalize(viewDir + lightDir);

    // 距离衰减
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (distance * distance);

    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    
    // diffuse
    float diffuseStrength = max(dot(lightDir, norm), 0);
    vec3 diffuse = diffuseStrength * objectColor;

    // specular
    float spec = pow(max(dot(halfDir, norm), 0), 32);
    vec3 specular = spec * lightColor * 0.5;
    
    vec3 finalColor = ambient * objectColor + attenuation * (specular + diffuse * objectColor);
    FragColor = vec4(finalColor, 1.0);
}
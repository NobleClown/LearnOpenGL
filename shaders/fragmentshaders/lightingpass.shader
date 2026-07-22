#version 330 core

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// 聚光灯
struct SpotLight {
    vec3 direction;
    vec3 position;
    float cutOff;
    float outterCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

// 平行光源 、 光源无限远
struct DirLigth {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// 点光源
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    float Radius;
};

vec3 CalcDirLight(DirLigth light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;
uniform DirLigth dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 cameraPos;
uniform SpotLight spotLight;

Material material;
float AmbientOcclusion;

void main() {
    // 基础向量
    vec3 norm = normalize(texture(gNormal, TexCoords).rgb);
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float alpha = texture(gAlbedoSpec, TexCoords).a;
    AmbientOcclusion = texture(ssao, TexCoords).r;
    material.shininess = 32;
    material.diffuse = albedo;
    material.specular = vec3(alpha, alpha, alpha);
    vec3 viewDir = cameraPos - FragPos;
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for (int i=0; i<NR_POINT_LIGHTS; i++) {
        float distance = length(pointLights[i].position - FragPos);
        if (distance < pointLights[i].Radius)
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLigth light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0);
    vec3 halfDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfDir, normal), 0), material.shininess);
    vec3 ambient = light.ambient * material.diffuse * AmbientOcclusion;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 halfDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfDir, normal), 0), material.shininess);

    vec3 ambient = light.ambient * material.diffuse * attenuation * AmbientOcclusion;
    vec3 diffuse = light.diffuse * diff * material.diffuse * attenuation;
    vec3 specular = light.specular * spec * material.specular * attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(-lightDir, light.direction);
    float epsilon = light.cutOff - light.outterCutOff;
    float intensity = clamp((theta - light.outterCutOff) / epsilon, 0.0, 1.0);
    
    vec3 halfDir = normalize(viewDir + lightDir);

    float distance = dot(light.position - fragPos, light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 ambient = light.ambient * material.diffuse * attenuation * AmbientOcclusion;

    float diffuseStrength = max(dot(lightDir, normal), 0);
    vec3 diffuse = diffuseStrength * light.diffuse * material.diffuse * attenuation * intensity;

    float specularStrength = 0.5;
    float spec = pow(max(dot(halfDir, normal), 0), 32) * specularStrength;
    vec3 specular = material.specular * spec * light.specular * attenuation * intensity;

    return ambient + diffuse + specular;
}
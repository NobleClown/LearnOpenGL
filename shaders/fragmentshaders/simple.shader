#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
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
};

vec4 sample_texture(vec2 coord, bool isDiffuse);
vec3 CalcDirLight(DirLigth light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoord;

uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform Material material;
uniform SpotLight spotLight;
uniform DirLigth dirLight;
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];

out vec4 FragColor;

float LinearizeDepth(float depth) {
    float near = 1.0;
    float far = 100.0;
    float z = depth * 2.0 - 1.0;
    return (near * far) / (far - z * (far - near));
}

void main() {
    // 基础向量
    vec3 norm = normalize(FragNormal);
    vec3 viewDir = normalize(cameraPos - FragPos);
    // vec3 result = CalcDirLight(dirLight, norm, viewDir);
    vec3 result = vec3(0.0, 0.0, 0.0);
    for (int i=0; i<NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
    // FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / 100), 1.0);
}

vec3 CalcDirLight(DirLigth light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0);
    vec3 halfDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfDir, normal), 0), material.shininess);
    vec3 ambient = light.ambient * vec3(sample_texture(TexCoord, true));
    vec3 diffuse = light.diffuse * diff * vec3(sample_texture(TexCoord, true));
    vec3 specular = light.specular * spec * vec3(sample_texture(TexCoord, false));
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 halfDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfDir, normal), 0), material.shininess);

    vec3 ambient = light.ambient * vec3(sample_texture(TexCoord, true)) * attenuation;
    vec3 diffuse = light.diffuse * diff * vec3(sample_texture(TexCoord, true)) * attenuation;
    vec3 specular = light.specular * spec * vec3(sample_texture(TexCoord, true)) * attenuation;

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

    vec3 ambient = light.ambient * vec3(sample_texture(TexCoord, true)) * attenuation;

    float diffuseStrength = max(dot(lightDir, normal), 0);
    vec3 diffuse = diffuseStrength * light.diffuse * vec3(sample_texture(TexCoord, true)) * attenuation * intensity;

    float specularStrength = 0.5;
    float spec = pow(max(dot(halfDir, normal), 0), 32) * specularStrength;
    vec3 specular = vec3(sample_texture(TexCoord, false)) * spec * light.specular * attenuation * intensity;

    return ambient + diffuse + specular;
}

vec4 sample_texture(vec2 coord, bool isDiffuse) {
    if (isDiffuse) {
        return texture(material.texture_diffuse1, coord) + 
               texture(material.texture_diffuse2, coord) +
               texture(material.texture_diffuse3, coord);
    } else {
        return texture(material.texture_specular1, coord) +
               texture(material.texture_specular2, coord);
    }
}
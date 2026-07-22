#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
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

vec3 CalcDirLight(DirLigth light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec2 ParallaxMapping(vec2 texCoordstmp, vec3 viewDir);

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoord;
in mat3 TBN;
in vec3 viewDir;

uniform vec3 cameraPos;
uniform Material material;
uniform SpotLight spotLight;
uniform DirLigth dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform sampler2D brick_normal;
uniform sampler2D brick_disp;
uniform float height_scale;
vec2 texCoords;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


void main() {
    // 基础向量
    vec3 TBNcameraPos = TBN * cameraPos;
    vec3 TBNFragPos = TBN * FragPos;
    texCoords = ParallaxMapping(TexCoord, viewDir);
    if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;
    vec3 norm = normalize(texture(brick_normal, texCoords).rgb * 2.0 - 1.0);
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for (int i=0; i<NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness >= 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
}

vec3 CalcDirLight(DirLigth light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-TBN * light.direction);
    float diff = max(dot(lightDir, normal), 0);
    vec3 halfDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfDir, normal), 0), material.shininess);
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(TBN * light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0);

    float distance = length(TBN * light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 halfDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfDir, normal), 0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords)) * attenuation;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords)) * attenuation;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords)) * attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(TBN * light.position - fragPos);
    float theta = dot(-lightDir, TBN * light.direction);
    float epsilon = light.cutOff - light.outterCutOff;
    float intensity = clamp((theta - light.outterCutOff) / epsilon, 0.0, 1.0);
    
    vec3 halfDir = normalize(viewDir + lightDir);

    float distance = dot(TBN * light.position - fragPos, TBN * light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords)) * attenuation;

    float diffuseStrength = max(dot(lightDir, normal), 0);
    vec3 diffuse = diffuseStrength * light.diffuse * vec3(texture(material.diffuse, texCoords)) * attenuation * intensity;

    float specularStrength = 0.5;
    float spec = pow(max(dot(halfDir, normal), 0), 32) * specularStrength;
    vec3 specular = vec3(texture(material.specular, texCoords)) * spec * light.specular * attenuation * intensity;

    return ambient + diffuse + specular;
}

vec2 ParallaxMapping(vec2 texCoordstmp, vec3 viewDir) {
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 0), viewDir)));
    float layerDepth = 1 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 p = viewDir.xy * height_scale;
    vec2 deltaTexCoords = p / numLayers;
    vec2 currentTexCoords = texCoordstmp;
    float currentDepthValue = texture(brick_disp, currentTexCoords).r;
    while (currentLayerDepth < currentDepthValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthValue = texture(brick_disp, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth = currentDepthValue - currentLayerDepth;
    float prevDepth = texture(brick_disp, prevTexCoords).r - currentLayerDepth + layerDepth;
    float weight = afterDepth / (afterDepth - prevDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1 - weight);
    return finalTexCoords;
}
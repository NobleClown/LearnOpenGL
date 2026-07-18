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
float depthTest(vec3 fragPos);

in vec3 FragPos;
in vec3 FragNormal;
in vec2 TexCoord;

uniform float farPlane;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform Material material;
uniform SpotLight spotLight;
uniform DirLigth dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform samplerCube shadowMap;

out vec4 FragColor;

void main() {
    // 基础向量
    vec3 norm = normalize(FragNormal);
    float inShadow = depthTest(FragPos);
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for (int i=0; i<NR_POINT_LIGHTS; i++) {
        result += (1.0 - inShadow) * CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLigth light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0);
    vec3 halfDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfDir, normal), 0), material.shininess);
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 halfDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfDir, normal), 0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord)) * attenuation;
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord)) * attenuation;
    vec3 specular = light.specular * spec * vec3(texture(material.diffuse, TexCoord)) * attenuation;

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

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord)) * attenuation;

    float diffuseStrength = max(dot(lightDir, normal), 0);
    vec3 diffuse = diffuseStrength * light.diffuse * vec3(texture(material.diffuse, TexCoord)) * attenuation * intensity;

    float specularStrength = 0.5;
    float spec = pow(max(dot(halfDir, normal), 0), 32) * specularStrength;
    vec3 specular = vec3(texture(material.specular, TexCoord)) * spec * light.specular * attenuation * intensity;

    return ambient + diffuse + specular;
}

float depthTest(vec3 fragPos) {
    vec3 sampleOffsetDirections[20] = vec3[] (
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(shadowMap, fragToLight).r * farPlane;
    float curDepth = length(fragToLight);
    float bias = max(0.05 * (1 - dot(normalize(FragNormal), normalize(-fragToLight))), 0.005);
    float shadow = 0;
    float samples = 20.0;
    float viewDis = length(cameraPos - fragPos);
    float diskRadius = (1.0 + (viewDis / farPlane)) / 25.0;
    for (int i=0; i<20; i++) {
        float closestDepth = texture(shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r * farPlane;
        if (curDepth - bias > closestDepth)
            shadow += 1.0;
    }
    return shadow / samples;
}
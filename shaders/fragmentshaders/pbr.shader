#version 330 core

struct PointLight {
    vec3 position;
    vec3 color;
};

out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 camPos;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMetalRough;
uniform sampler2D ssao;

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

const float PI = 3.14159265359;

// 法线分布函数，用于计算微表面法线与表面半程向量的贴近程度
// n:法线，h:半程向量，a:粗糙程度
float D_GGX_TR(vec3 N, vec3 H, float roughless) {
    float a = roughless * roughless;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

// 几何函数，计算微表面之间互相遮挡的比率
// n:法线, v:观察方向, l:光线方向, k粗糙度的重映射
float G_S_GGX(float NdotV, float roughless) {
    float r = roughless + 1.0;
    float k = r * r / 8.0;
    float nom =  NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float G_Smith(vec3 N, vec3 V, vec3 L, float roughless) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = G_S_GGX(NdotV, roughless);   // 观察方向上的几何遮蔽(看不到)
    float ggx2 = G_S_GGX(NdotL, roughless);   // 光线方向上的几何阴影(阴影中)

    return ggx1 * ggx2;
}

// 菲涅尔方程，反映反射与折射之间的比率
// n: 法线, v: 观察方向, surfaceColor: 表面颜色, metalness: 金属度
vec3 F_Schlick(float cosTheta, vec3 F0) {
    return F0 + (1 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 Fresnel(vec3 N, vec3 V, vec3 albedo, float metalness) {
    vec3 F0 = vec3(0.04);   // 电介质近似基础反射率
    F0 = mix(F0, albedo, metalness);
    float cosTheta = dot(N, V);
    return F_Schlick(cosTheta, F0);
}

void main() {
    vec3 Normal = texture(gNormal, TexCoords).xyz;
    vec3 WorldPos = texture(gPosition, TexCoords).xyz;
    vec3 albedo = texture(gAlbedoSpec, TexCoords).rgb;
    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);
    vec3 Lo = vec3(0.0);
    float ao = texture(ssao, TexCoords).r;
    float roughless = texture(gMetalRough, TexCoords).g;
    float metallic = texture(gMetalRough, TexCoords).r;
    for (int i=0; i<4; i++) {
        vec3 L = normalize(pointLights[i].position - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color * attenuation;
        // 计算近似的反射率和折射率
        vec3 F = Fresnel(N, V, albedo, metallic);
        // 根据微表面法线分布计算法线与半程向量的接近程度
        float NDF = D_GGX_TR(N, H, roughless);
        // 计算微表面互相遮挡的程度
        float G = G_Smith(N, V, L, roughless);
        vec3 nominator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(L, V), 0.0) + 0.001;
        vec3 specular = nominator / denominator;
        // F给出反射率，可据此计算折射率，如果是金属，则置为0（金属不进行漫反射，吸收所有折射光）
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        // 根据入射光方向与法线夹角计算光线的贡献率
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // 环境光
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    // 色调映射和gamma矫正
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
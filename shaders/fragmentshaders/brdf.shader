#version 330 core

in vec3 TexCoords;

uniform sampler2D loft;

out vec4 FragColor;

const float PI = 3.14159265359;

float RadicalInverse_Vdc(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 1u) | ((bits & 0xCCCCCCCCu) >> 1u);
    bits = ((bits & 0x0F0F0F0Fu) << 1u) | ((bits & 0xF0F0F0F0u) >> 1u);
    bits = ((bits & 0x00FF00FFu) << 1u) | ((bits & 0xFF00FF00u) >> 1u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverse_Vdc(i));
}
// 法线分布函数，用于计算微表面法线与表面半程向量的贴近程度
// n:法线，h:半程向量，a:粗糙程度
float D_GGX_TR(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
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
float G_S_GGX(float NdotV, float roughness) {
    float r = roughness;
    float k = r * r / 2.0;
    float nom =  NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float G_Smith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = G_S_GGX(NdotV, roughness);   // 观察方向上的几何遮蔽(看不到)
    float ggx2 = G_S_GGX(NdotL, roughness);   // 光线方向上的几何阴影(阴影中)

    return ggx1 * ggx2;
}

// 将采样的Xi转换为对应的方向向量
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness;
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0):vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

vec2 IntegrateBRDF(float NdotV, float roughness) {
    vec3 V;
    V.x = sqrt(1.0 - NdotV * NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for (uint i=0u; i<SAMPLE_COUNT; i++) {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0) {
            float G = G_Smith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}

void main() {
    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);
    FragColor = integratedBRDF;
}
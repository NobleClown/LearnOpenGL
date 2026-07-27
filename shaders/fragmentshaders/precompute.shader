#version 330 core

in vec3 TexCoords;

uniform sampler2D loft;

out vec4 FragColor;

// 分别为1/pi和1/2pi
const vec2 invAtan = vec2(0.1591, 0.3183);

// 方向向量转换为极坐标的过程（xyz->theta phi）
vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(TexCoords));
    vec3 color = texture(loft, uv).rgb;
    FragColor = vec4(color, 1.0);
}
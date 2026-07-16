#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position) {
    vec2 offsets[5];
    offsets[0] = vec2(-0.2, -0.2);
    offsets[0] = vec2( 0.2, -0.2);
    offsets[0] = vec2(-0.2,  0.2);
    offsets[0] = vec2( 0.2,  0.2);
    offsets[0] = vec2( 0.0,  0.4);

    for (int i=0; i<5; i++) {
        gl_Position = position + vec4(offsets[i].xy, 0.0, 0.0);
        if (i == 4)
            fColor = vec3(1.0, 1.0, 1.0);
        EmitVertex();
    }

    EndPrimitive();
}

void main() {

    fColor = gs_in[0].color;

    build_house(gl_in[0].gl_Position);
}
#version 460 core

in vec4 v_Color;
in vec2 v_UV;
in vec3 v_WorldPos;
uniform sampler2D u_Texture;
layout(location = 0) out vec4 color;

void main() {
    vec4 baseColor = texture(u_Texture, v_UV) * v_Color;
    if (baseColor.a < 0.05) {
        discard;
    }
    color = baseColor;
}

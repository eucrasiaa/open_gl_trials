#version 460 core

layout(location = 0) out vec4 color;
in vec4 v_Color;

void main() {
    // color = v_Color;
    color=vec4(0.0f,0.0f,0.0f,1.0f);
}

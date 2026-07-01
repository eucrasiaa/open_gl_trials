#version 460 core
layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_UV;

uniform sampler2D u_Texture; 

void main() {
  color = vec4(v_UV.x, v_UV.y, 1.0, 1.0);
  // color = texture(u_Texture, v_UV) * v_Color;
}

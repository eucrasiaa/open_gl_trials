#version 460 core
#extension GL_ARB_bindless_texture : require
#extension GL_ARB_gpu_shader_int64 : require
layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_UV;

flat in uint64_t   v_TextureHandle64; 
// uniform sampler2D u_Texture; 

void main() {
  // color = vec4(v_UV.x, v_UV.y, 1.0, 1.0);
  sampler2D activeSampler = sampler2D(v_TextureHandle64);
  color = texture(activeSampler, v_UV) * v_Color;
  // color = texture(u_Texture, v_UV) * v_Color;
  // color = texture(u_Texture, v_UV);
}

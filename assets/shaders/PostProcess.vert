#version 330 core
// layout (location = 0) in vec2 aPos;
// layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main(){
  // one BIIIG triangle
  float x = -1.0 + float((gl_VertexID & 1) << 2);
  float y = -1.0 + float((gl_VertexID & 2) << 1);

  gl_Position = vec4(x, y, 0.0, 1.0);
  TexCoords = vec2((x + 1.0) * 0.5, (y + 1.0) * 0.5);

  // vec2 positions[3] = vec2[](
  //     vec2(-1.0, -1.0),
  //     vec2( 3.0, -1.0),
  //     vec2(-1.0,  3.0)
  //     );
  //
  //    vec2 pos = vec2(
  //       (gl_VertexID == 1) ?  3.0 : -1.0,
  //       (gl_VertexID == 2) ?  3.0 : -1.0
  //   );
  // gl_Position = vec4(pos, 0.0, 1.0);
  // TexCoords = pos * 0.5 + 0.5;
}

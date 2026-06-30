#version 330 core
// layout (location = 0) in vec2 aPos;
// layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main(){
  // one BIIIG triangle
  vec2 positions[3] = vec2[](
      vec2(-1.0, -1.0),
      vec2( 3.0, -1.0),
      vec2(-1.0,  3.0)
      );

     vec2 pos = vec2(
        (gl_VertexID == 1) ?  3.0 : -1.0,
        (gl_VertexID == 2) ?  3.0 : -1.0
    );
    gl_Position = vec4(pos, 0.0, 1.0);
    TexCoords = pos * 0.5 + 0.5;
}

#version 460 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_UV;
layout(location = 3) in float a_TexIndex;

//4,5,6,7
layout (location = 4) in mat4 aInstanceMatrix;

// uniform mat4 u_Model;
// uniform mat4 u_View;
uniform mat4 u_Projection;

out vec4 v_Color;
out vec2 v_UV;

void main() {
    v_Color = a_Color;
    v_UV = a_UV;
    // gl_Position = vec4(a_Position,1.0);
    // project * instance * local verts 
    gl_Position = u_Projection * a_InstanceMatrix * vec4(a_Position, 1.0);
    // gl_Position = u_Projection * vec4(a_Position, 1.0);
    // gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}

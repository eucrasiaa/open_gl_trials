#version 460 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_UV;
layout(location = 3) in vec3 a_Normal;
layout(location = 4) in float a_TexIndex;

//4,5,6,7
layout (location = 5) in mat4 a_InstanceMatrix;

// uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform float u_Thick;

out vec4 v_Color;
out vec2 v_UV;

void main() {
    v_Color = a_Color;
    v_UV = a_UV;

    // Manual matrix multiplication test
    // column 0, column 1, column 2 multiplied by vertex position + column 3 translation
    vec3 worldPos = mat3(a_InstanceMatrix) * a_Position + a_InstanceMatrix[3].xyz;

    gl_Position = u_Projection * u_View * vec4(worldPos, 1.0);
}

// void main() {
//     v_Color = a_Color;
//     v_UV = a_UV;
//     // gl_Position = vec4(a_Position,1.0);
//     // project * instance * local verts 
//     gl_Position = u_Projection * u_View * a_InstanceMatrix * vec4(a_Position, 1.0);
//     // gl_Position = u_Projection * vec4(a_Position, 1.0);
//     // gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
// }

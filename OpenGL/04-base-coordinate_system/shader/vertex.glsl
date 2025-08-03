#version 460 core

layout (location = 0) in  vec3 a_pos;
layout (location = 1) in  vec2 a_tex_coord;
layout (location = 0) out vec2 v_tex_coord;

uniform mat4 u_model_mat;
uniform mat4 u_view_mat;
uniform mat4 u_projection_mat;

void main()
{
    gl_Position = u_projection_mat * u_view_mat * u_model_mat * vec4(a_pos, 1.0);
    v_tex_coord = a_tex_coord;
}
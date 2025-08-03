#version 460 core

layout (location = 0) in  vec3 a_pos;
layout (location = 1) in  vec3 a_color;
layout (location = 2) in  vec2 a_tex_coord;
layout (location = 0) out vec3 v_color;
layout (location = 1) out vec2 v_tex_coord;

void main()
{
    gl_Position = vec4(a_pos, 1.0);
    v_color = a_color;  // 测试注释掉，shader 是否会异常
    v_color = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
    v_tex_coord = a_tex_coord;
}
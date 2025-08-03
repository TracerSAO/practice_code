#version 460 core

layout (location = 0) in  vec3 a_pos;
layout (location = 0) out vec3 v_color;

void main()
{
    gl_Position = vec4(a_pos, 1.0);
    v_color = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
}
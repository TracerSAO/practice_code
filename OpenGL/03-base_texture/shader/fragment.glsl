#version 460 core

layout (location = 0) in  vec3 a_color;
layout (location = 0) out vec4 f_color;

void main()
{
    f_color = vec4(a_color, 1.0);
}
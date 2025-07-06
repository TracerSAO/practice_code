#version 460

layout (location = 0) in  vec4 a_color;
layout (location = 0) out vec4 v_color;

void main()
{
    v_color = a_color;
}
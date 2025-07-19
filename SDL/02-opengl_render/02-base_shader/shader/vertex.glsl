#version 460 core

layout (location = 0) in  vec3 a_pos;
layout (location = 1) in  vec3 a_color;
layout (location = 0) out vec3 v_color;

uniform float x_pos_offset;

vec3 flipBaseX(const vec3 pos)
{
    return vec3(pos.x, -1 * pos.y, pos.z);
}

vec3 updaeOffsetBaseX(const vec3 pos, float x_pos_offset)
{
    return vec3(pos.x + x_pos_offset, pos.y, pos.z);
}

void main()
{
    gl_Position = vec4(a_pos, 1.0);
    // gl_Position = vec4(updaeOffsetBaseX(a_pos, x_pos_offset), 1.0);
    // gl_Position = vec4(flipBaseX(updaeOffsetBaseX(a_pos, x_pos_offset)), 1.0);
    v_color = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
    // v_color = a_color;
}
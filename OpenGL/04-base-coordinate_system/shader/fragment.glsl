#version 460 core

layout (location = 0) in  vec3 v_color;
layout (location = 1) in  vec2 v_tex_coord;
layout (location = 0) out vec4 f_color;

uniform texture2D u_backgroud_tex;
uniform texture2D u_preview_tex;

void main()
{
    // f_color = vec4(v_color, 1.0);
    f_color = min(texture(u_backgroud_tex, v_tex_coord), texture(u_preview_tex, v_tex_coord), 0.8);
}
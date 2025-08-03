#version 460 core

layout (location = 0) in  vec2 v_tex_coord;
layout (location = 0) out vec4 f_color;

uniform sampler2D u_backend_tex0;
uniform sampler2D u_frontend_tex1;

vec2 flipTexXCoord(vec2 tex_coord) {
    return vec2(1.0 - tex_coord.x, tex_coord.y);
}

void main()
{
    f_color = mix(texture(u_backend_tex0, v_tex_coord), texture(u_frontend_tex1, flipTexXCoord(v_tex_coord)), 0.3);
    // f_color = mix(texture(u_backend_tex0, v_tex_coord), texture(u_frontend_tex1, v_tex_coord), 0.3);
}
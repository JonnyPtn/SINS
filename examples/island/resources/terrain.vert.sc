$input a_position, a_texcoord0, a_color0
$output v_texcoord0, v_color0, v_normal

#include <bgfx_shader.sh>

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
    v_color0 = a_color0;
    v_normal = vec4(a_texcoord0.xy, 1.0,1.0);
}

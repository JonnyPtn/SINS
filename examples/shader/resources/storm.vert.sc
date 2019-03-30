$input a_position, a_texcoord0, a_color0
$output v_texcoord0, v_color0

#include <bgfx_shader.sh>

uniform vec4 storm_position;
uniform vec4 storm_total_radius;
uniform vec4 storm_inner_radius;

void main()
{
    vec4 vertex = mul(u_modelView, vec4(a_position, 0.0) );
    vec2 offset = vertex.xy - storm_position.xy;
    float len = length(offset);
    if (len < storm_total_radius[0])
    {
        float push_distance = storm_inner_radius[0] + len / storm_total_radius[0] * (storm_total_radius[0] - storm_inner_radius[0]);
        vertex.xy = storm_position.xy + normalize(offset) * push_distance;
    }

    gl_Position = mul(u_proj, vertex );
    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
}

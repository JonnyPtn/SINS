$input a_position, a_texcoord0, a_color0
$output v_texcoord0, v_color0

#include <bgfx_shader.sh>

uniform vec4 wave_phase;
uniform vec4 wave_amplitude;

void main()
{
    vec4 vertex = vec4(a_position, 1.0);
    vertex.x += cos(a_position.y * 0.02 + wave_phase[0] * 3.8) * wave_amplitude.x
              + sin(a_position.y * 0.02 + wave_phase[0] * 6.3) * wave_amplitude.x * 0.3;
    vertex.y += sin(a_position.x * 0.02 + wave_phase[0] * 2.4) * wave_amplitude.y
              + cos(a_position.x * 0.02 + wave_phase[0] * 5.2) * wave_amplitude.y * 0.3;

    gl_Position = mul(u_modelViewProj, vertex );
    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
}

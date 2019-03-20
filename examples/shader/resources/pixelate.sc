$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor,0);
uniform vec4 pixel_threshold;

void main()
{
    float factor = 1.0 / (pixel_threshold[0] + 0.001);
    vec2 pos = floor(v_texcoord0.xy * factor + 0.5) / factor;
    gl_FragColor = texture2D(s_texColor, pos) * v_color0;
}

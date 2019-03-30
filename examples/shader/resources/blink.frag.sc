$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor,0);
uniform vec4 blink_alpha;

void main()
{
    vec4 pixel = v_color0;
    pixel.a = blink_alpha[0];
    gl_FragColor = pixel;
}

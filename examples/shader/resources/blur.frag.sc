$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);
uniform vec4 blur_radius;

void main()
{
    vec2 offx = vec2(blur_radius[0], 0.0);
    vec2 offy = vec2(0.0, blur_radius[0]);

    vec4 pixel = texture2D(s_texColor, v_texcoord0.xy)               * 4.0 +
                 texture2D(s_texColor, v_texcoord0.xy - offx)        * 2.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offx)        * 2.0 +
                 texture2D(s_texColor, v_texcoord0.xy - offy)        * 2.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offy)        * 2.0 +
                 texture2D(s_texColor, v_texcoord0.xy - offx - offy) * 1.0 +
                 texture2D(s_texColor, v_texcoord0.xy - offx + offy) * 1.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offx - offy) * 1.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offx + offy) * 1.0;

    gl_FragColor =  v_color0 * (pixel / 16.0);
}

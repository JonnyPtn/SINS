$input v_texcoord0, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);
uniform vec4 edge_threshold;

void main()
{
    const float offset = 1.0 / 512.0;
    vec2 offx = vec2(offset, 0.0);
    vec2 offy = vec2(0.0, offset);

    vec4 hEdge = texture2D(s_texColor, v_texcoord0.xy - offy)        * -2.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offy)        *  2.0 +
                 texture2D(s_texColor, v_texcoord0.xy - offx - offy) * -1.0 +
                 texture2D(s_texColor, v_texcoord0.xy - offx + offy) *  1.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offx - offy) * -1.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offx + offy) *  1.0;

    vec4 vEdge = texture2D(s_texColor, v_texcoord0.xy - offx)        *  2.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offx)        * -2.0 +
                 texture2D(s_texColor, v_texcoord0.xy - offx - offy) *  1.0 +
                 texture2D(s_texColor, v_texcoord0.xy - offx + offy) * -1.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offx - offy) *  1.0 +
                 texture2D(s_texColor, v_texcoord0.xy + offx + offy) * -1.0;

    vec3 result = sqrt(hEdge.rgb * hEdge.rgb + vEdge.rgb * vEdge.rgb);
    float edge = length(result);
    vec4 pixel = v_color0 * texture2D(s_texColor, v_texcoord0.xy);
    if (edge > (edge_threshold[0] * 8.0))
        pixel.rgb = vec3(0.0, 0.0, 0.0);
    else
        pixel.a = edge_threshold[0];
    gl_FragColor = pixel;
}

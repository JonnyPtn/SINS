$input v_texcoord0, v_color0, v_normal

#include <bgfx_shader.sh>

uniform vec4 lightFactor;

void main()
{
    vec3 lightPosition = vec3(-1.0, 1.0, 1.0);
    vec3 eyePosition = vec3(0.0, 0.0, 1.0);
    vec3 halfVector = normalize(lightPosition + eyePosition);
    vec4 intensity = lightFactor + (1.0 - lightFactor) * dot(normalize(v_normal.xyz), normalize(halfVector));
    gl_FragColor = v_color0 * vec4(intensity.x, intensity.x, intensity.x, 1.0);
}

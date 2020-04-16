#version 460 core
#define SOLID_COLOR 1
#define NO_SHADE 4

in vec3 col;
in vec2 uv;
in vec3 normal_cs;
in vec3 light_dir_cs;
in vec3 pos;


uniform sampler2D tex_sampler;
uniform vec3 light_pos;
uniform int flags;
uniform vec3 u_col;

out vec4 fragColor;

float light_power = 3;

void main() {
    vec4 mcol = (flags & SOLID_COLOR) == 0 ? texture(tex_sampler, uv).rgba : vec4(u_col, 1);
    if ((flags & NO_SHADE) == 0) {
        vec3 diff_col = mcol.rgb;
        vec3 amb_col = 0.2 * diff_col;

        float dist = length(light_pos - pos);

        vec3 n = normalize(normal_cs);
        vec3 l = normalize(light_dir_cs);
        float cos_theta = clamp(dot(n, l), 0, 1);
        
        vec3 col1 = amb_col + light_power * cos_theta * diff_col / (dist * dist);
        fragColor = vec4(col1, mcol.a);
    } else {
        fragColor = mcol;
    }
}
#version 460 core

in vec3 col;
in vec2 uv;

uniform sampler2D tex_sampler;

out vec4 fragColor;
void main(){
    fragColor = texture(tex_sampler, uv).rgba;
}
#version 460 core
#define NO_BONES 2

layout (location=0) in vec3 a_pos;
layout (location=1) in uvec4 a_joi;
layout (location=2) in vec4 a_wei;
layout (location=3) in vec2 a_uv;
layout (location=4) in vec3 a_nor;

uniform mat4 mvp;
uniform mat4 v;
uniform mat4 m;

layout(std140,binding=3) buffer Bones {
    mat4 bones[];
};
uniform int use_bones;

uniform int flags;

out vec2 uv;
out vec3 normal_cs;
out vec3 pos;

void main(){
    uv = a_uv;
    mat4 jm = bones[a_joi.x]*a_wei.x + bones[a_joi.y]*a_wei.y + bones[a_joi.z]*a_wei.z + bones[a_joi.w]*a_wei.w;
    vec4 vertex_pos = (flags & NO_BONES) == 0 ? jm * vec4(a_pos, 1) : vec4(a_pos, 1);
    pos = vertex_pos.xyz;

    normal_cs = (v * m * jm * vec4(a_nor, 0)).xyz;

    gl_Position = mvp * vertex_pos;
    
}
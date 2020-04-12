#version 460 core
layout (location=0) in vec3 a_pos;
layout (location=1) in uvec4 a_joi;
layout (location=2) in vec4 a_wei;

uniform mat4 mvp;
layout(std140,binding=3) buffer Bones {
    mat4 bones[];
};
uniform int use_bones;
uniform vec3 u_col;
//uniform float a;

out vec3 col;

void main(){
    col = u_col;
    if (use_bones==1) {
        mat4 jm = bones[a_joi.x]*a_wei.x + bones[a_joi.y]*a_wei.y + bones[a_joi.z]*a_wei.z + bones[a_joi.w]*a_wei.w;
        float sum = a_wei.x + a_wei.y + a_wei.z + a_wei.w;
        float a = 0.01;
        gl_Position = mvp * (mat4(1) * a + jm * (1-a)) * vec4(a_pos, 1);
    } else {
        gl_Position = mvp * vec4(a_pos, 1);
    }
    
}
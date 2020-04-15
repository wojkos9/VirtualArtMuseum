#version 460 core
layout (location=0) in vec3 a_pos;
layout (location=1) in vec2 a_wei;
layout (location=2) in ivec2 idx;

layout(std140,binding=3) buffer Bones {
    mat4 bones[];
};


out vec3 col;

void main(){

    col = vec3(idx.x, idx.y, 0);
    //gl_Position = ((idx.x==0?bones.data[0]:bones.data[1])*a_wei.x+(idx.y==0?bones.data[0]:bones.data[1])*a_wei.y) * vec4(a_pos, 1);
    gl_Position = (bones[idx.x]*a_wei.x+bones[idx.y]*a_wei.y) * vec4(a_pos, 1);
}
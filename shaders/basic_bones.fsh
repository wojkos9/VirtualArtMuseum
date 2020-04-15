#version 460 core

in vec3 col;

out vec4 fragColor;
void main(){
    fragColor=vec4(col, 1);
}
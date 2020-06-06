#version 460 core
 layout (location = 0) in vec3 aPos;
 layout (location = 1) in vec3 aTexCoord;

uniform mat4 mvp;
// uniform mat4 v;
// uniform mat4 m;
out vec2 TexCoord;
void main()
{
    gl_Position = mvp* vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoord=vec2(aTexCoord.x,aTexCoord.y);
}
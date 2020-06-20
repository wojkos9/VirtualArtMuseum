#version 460 core
 layout (location = 0) in vec3 aPos;
 layout (location = 1) in vec3 aNormal;
 layout (location =2) in vec2 aTexCoord;
 
uniform mat4 mvp;
uniform mat4 v;
uniform mat4 m;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    FragPos = vec3(m * vec4(aPos, 1.0));
    //FragPos =  aPos;
    gl_Position = mvp* vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoord=vec2(aTexCoord.x,aTexCoord.y);
    Normal =mat3(transpose(inverse(m))) *aNormal;

}
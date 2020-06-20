#version 460 core
layout (location = 0) in vec3 aPos;


uniform mat4 mvp;
//  uniform mat4 v;
//  uniform mat4 m;
void main()
{
	 gl_Position = mvp* vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
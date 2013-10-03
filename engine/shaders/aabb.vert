// AABB vertex shader

#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 gMVP;

void main()
{
	gl_Position = gMVP * vec4(position, 1.0);
}
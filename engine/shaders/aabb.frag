// AABB fragment shader

#version 330 core

layout(location = 0) out vec4 fragColor;

uniform vec3 gColor;

void main()
{
	fragColor = vec4(gColor, 1.0);
}
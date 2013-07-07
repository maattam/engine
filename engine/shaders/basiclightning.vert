// Basic lightning vertex shader

#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

uniform mat4 gMVP;
uniform mat4 gLightMVP;
uniform mat4 gWorld;
uniform bool gHasTangents;

out vec2 texCoord0;
out vec3 normal0;
out vec3 tangent0;
out vec3 worldPos0;
out vec4 lightSpacePos0;

void main()
{
	gl_Position = gMVP * vec4(position, 1.0);
	
	texCoord0 = texCoord;
	normal0 = (gWorld * vec4(normal, 0.0)).xyz;
	worldPos0 = (gWorld * vec4(position, 1.0)).xyz;
	lightSpacePos0 = gLightMVP * vec4(position, 1.0);
	
	if(gHasTangents)
	{
		tangent0 = (gWorld * vec4(tangent, 0.0)).xyz;
	}
}
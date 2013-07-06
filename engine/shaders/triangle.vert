#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 vertexTangent_modelspace;

uniform vec3 LightPosition_worldspace;
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

out vec2 texcoord;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out float distance;

void main() {
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
	texcoord = vertexUV;

	vec3 Position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;

	// Vector that goes from the vertex to the camera
	// In camera space, the camera is at the origin
	vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition_modelspace, 1)).xyz;
	EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;

	// Vector that goes from the vertex to the light
	vec3 LightPosition_cameraspace = (V * vec4(LightPosition_worldspace, 1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

	Normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0)).xyz;
}

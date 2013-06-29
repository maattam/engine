#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

out vec3 pos_fragment;

void main() {
	gl_Position.xyz = vertexPosition_modelspace;
	gl_Position.w = 1.0;

	pos_fragment = vec3(gl_Position.xyz);
}

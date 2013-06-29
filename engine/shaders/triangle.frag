#version 330 core

out vec3 color;

varying vec3 pos_fragment;

void main() {
	color = vec3(pos_fragment.x, pos_fragment.y, 0.5);
}
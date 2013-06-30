#version 330 core

uniform sampler2D texture;
in vec2 texcoords;

out vec3 color;

void main() {
	//color = texture2D(texture, texcoords).rgb;
	color = vec3(texcoords, 1);
}

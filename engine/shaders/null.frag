#version 330 core

uniform sampler2D tex;
in vec2 uv;

void main() {
	gl_FragColor = texture2D(tex, uv);
}
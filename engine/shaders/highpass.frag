#version 330 core

uniform sampler2D renderedTexture;
in vec2 uv;

uniform float brightThreshold;

void main() {
	vec4 color = texture2D(renderedTexture, uv);
	
	if (color.r > brightThreshold || color.g > brightThreshold || color.b > brightThreshold)
		gl_FragColor = color;
	else
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}
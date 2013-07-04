#version 330 core

uniform sampler2D renderedTexture;
uniform sampler2D bloomTexture;

uniform float exposure;
uniform float bloomFactor;
uniform float brightMax;

in vec2 uv;

void main() {
	vec4 color = texture(renderedTexture, uv);
	vec4 colorBloom = texture(bloomTexture, uv);

	// Add bloom
	color += colorBloom * bloomFactor;

	// Perform tone-mapping
	float YD = exposure * (exposure/brightMax + 1.0) / (exposure + 1.0);
	color *= YD;

	gl_FragColor = color;
}
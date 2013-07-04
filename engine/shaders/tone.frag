#version 330 core

uniform sampler2DMS renderedTexture;
uniform sampler2D bloomTexture;

uniform float exposure;
uniform float bloomFactor;
uniform float brightMax;

uniform int samples;

in vec2 uv;

void main() {
	ivec2 st = ivec2(textureSize(renderedTexture) * uv);
	vec4 colorBloom = texture(bloomTexture, uv);
	vec4 color;

	for(int i = 0; i < samples; ++i)
	{
		color += texelFetch(renderedTexture, ivec2(st), i);

		// Add bloom
		color += colorBloom * bloomFactor;

		// Perform tone-mapping
		float YD = exposure * (exposure/brightMax + 1.0) / (exposure + 1.0);
		color *= YD;
	}

	color = color / samples;

	gl_FragColor = color;
}
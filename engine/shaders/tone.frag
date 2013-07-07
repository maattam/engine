#version 330 core

const int BLOOM_SAMPLES = 4;

uniform sampler2DMS renderedTexture;
uniform sampler2D bloomSamplers[BLOOM_SAMPLES];

uniform float exposure;
uniform float bloomFactor;
uniform float brightMax;

uniform int samples;

in vec2 uv;

// Blend bloom samples
vec4 calcBloomColor()
{
	vec4 color = vec4(0, 0, 0, 0);

	for(int i = 0; i < BLOOM_SAMPLES; ++i)
	{
		color += texture(bloomSamplers[i], uv);
	}

	return color;
}

void main() {
	ivec2 st = ivec2(textureSize(renderedTexture) * uv);
	vec4 colorBloom = calcBloomColor();
	vec4 color;

	for(int i = 0; i < samples; ++i)
	{
		color += texelFetch(renderedTexture, ivec2(st), i);

		// Perform tone-mapping
		float YD = exposure * (exposure/brightMax + 1.0) / (exposure + 1.0);
		color *= YD;
	}

	color = color / samples;
	
	// Add bloom
	color += colorBloom * bloomFactor;

	gl_FragColor = color;
}
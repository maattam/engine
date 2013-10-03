#version 330 core

uniform sampler2DMS renderedTexture;

uniform sampler2D bloomSampler;
uniform int bloomLevels;

uniform float exposure;
uniform float bloomFactor;
uniform float brightMax;

uniform int samples;

in vec2 uv;

// Blend bloom samples
vec4 calcBloomColor()
{
	vec4 color = vec4(0, 0, 0, 0);

    // Skip first lod level to reduce aliasing
	for(int i = 1; i <= bloomLevels; ++i)
	{
		color += textureLod(bloomSampler, uv, i);
	}

	return color;
}

void main() {
	ivec2 st = ivec2(textureSize(renderedTexture) * uv);
	vec4 colorBloom = calcBloomColor();
	vec4 color = vec4(0, 0, 0, 0);

	for(int i = 0; i < samples; ++i)
	{
		color += texelFetch(renderedTexture, st, i);

		// Perform tone-mapping
		float YD = exposure * (exposure/brightMax + 1.0) / (exposure + 1.0);
		color *= YD;
	}

	color = color / samples;
	
	// Add bloom
	color += colorBloom * bloomFactor;

	gl_FragColor = color;
}
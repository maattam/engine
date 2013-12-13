#version 420

uniform sampler2D renderedTexture;

uniform sampler2D bloomSampler;
uniform int bloomLevels;

uniform float exposure;
uniform float bloomFactor;
uniform float bright;

in vec2 uv;

// Magic
float A = 0.30;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;

vec3 tonemap(in vec3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

// Blend bloom samples
vec3 calcBloomColor()
{
	vec3 color = vec3(0, 0, 0);

    // Skip first lod level to reduce aliasing
	for(int i = 1; i <= bloomLevels; ++i)
	{
		color += textureLod(bloomSampler, uv, i).rgb;
	}

	return color;
}

void main()
{
    // Bloom is calculated in srgb space to reduce floating point precision error
	vec3 colorBloom = pow(calcBloomColor(), vec3(2.2));
	vec3 color = texture(renderedTexture, uv).rgb;

	// Add bloom
	color += colorBloom * bloomFactor;

    color *= exposure;

    float expBias = 2.0f;
    vec3 current = tonemap(expBias * color.rgb);

    vec3 whiteScale = 1.0 / tonemap(vec3(bright));
    vec3 tone = current * whiteScale;

    // Map from linear color space to srgb with gamma 2.2
    gl_FragColor.rgb = pow(tone, vec3(1/2.2));
}
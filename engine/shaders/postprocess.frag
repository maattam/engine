#version 420

// A post-process compositor which consists of tonemapping, bloom blending and gamma correction.

#define SAMPLES <>
#define BLOOMLOD <>

uniform sampler2DMS inputTexture;
uniform sampler2D bloomTexture;

uniform float bloomFactor;
uniform float exposure;
uniform float bright;   // Linear white point value
uniform float gamma;

in vec2 uv;

// Filmic tonemapper curve parameters
float A = 0.30; // Shoulder strength
float B = 0.50; // Linear strength
float C = 0.10; // Linear angle
float D = 0.20; // Toe strength
float E = 0.02; // Toe numerator
float F = 0.30; // Toe Denominator

vec3 tonemap(in vec3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

// Blend bloom samples
vec3 calcBloomColor()
{
	vec3 color = vec3(0, 0, 0);

    // Skip first lod level to reduce aliasing
	for(int i = 0; i <= BLOOMLOD; ++i)
	{
		color += textureLod(bloomTexture, uv, i).rgb;
	}

	return color / BLOOMLOD;
}

void main()
{
	vec3 color = vec3(0, 0, 0);

    ivec2 st = ivec2(textureSize(inputTexture) * uv);
    for(int i = 0; i < SAMPLES; ++i)
    {
        color += texelFetch(inputTexture, st, i).rgb;
    }

    color /= SAMPLES;

	// Add bloom
	color += calcBloomColor() * bloomFactor;

    color *= exposure;

    float expBias = 2.0f;
    vec3 current = tonemap(expBias * color.rgb);

    vec3 whiteScale = 1.0 / tonemap(vec3(bright));
    vec3 tone = current * whiteScale;

    // Apply gamma correction
    gl_FragColor.rgb = pow(tone, vec3(1.0 / gamma));
}
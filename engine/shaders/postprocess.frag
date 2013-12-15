#version 420

// A post-process compositor which consists of tonemapping, bloom blending and gamma correction.

uniform sampler2DMS inputTexture;
uniform int samples;

uniform sampler2D bloomTexture;
uniform int bloomLevels;

uniform float bloomFactor;
uniform float exposure;
uniform float bright;
uniform float gamma;

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
		color += textureLod(bloomTexture, uv, i).rgb;
	}

    // Bloom is converted to srgb to mask some aliasing on blur edges
	return pow(color, vec3(2.2));
}

void main()
{
	vec3 color = vec3(0, 0, 0);

    ivec2 st = ivec2(textureSize(inputTexture) * uv);
    for(int i = 0; i < samples; ++i)
    {
        color += texelFetch(inputTexture, st, i).rgb;
    }

    color /= samples;

	// Add bloom
	color += calcBloomColor() * bloomFactor;

    color *= exposure;

    float expBias = 2.0f;
    vec3 current = tonemap(expBias * color.rgb);

    vec3 whiteScale = 1.0 / tonemap(vec3(bright));
    vec3 tone = current * whiteScale;

    // Apply gamma correction
    gl_FragColor.rgb = pow(tone, vec3(1/gamma));
}
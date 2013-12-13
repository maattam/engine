#version 420

uniform sampler2D renderedTexture;
uniform float threshold;

in vec2 uv;

void main()
{
    // Bloom is calculated in srgb space to reduce floating point precision error
	vec3 color = pow(texture(renderedTexture, uv).rgb, vec3(1/2.2));

    vec3 bright = max(color - vec3(threshold), vec3(0));
    float c = dot(bright, vec3(1.0));
    bright = smoothstep(0.0, 0.5, vec3(c));

    gl_FragColor.rgb = mix(vec3(0), color, bright);
}
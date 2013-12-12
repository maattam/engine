#version 420

uniform sampler2DMS renderedTexture;
in vec2 uv;

uniform float threshold;

void main() {
	ivec2 st = ivec2(textureSize(renderedTexture) * uv);

    // Bloom is calculated in srgb space to reduce floating point precision error
	vec3 color = pow(texelFetch(renderedTexture, st, gl_SampleID).rgb, vec3(1/2.2));

    vec3 bright = max(color - vec3(threshold), vec3(0));
    float c = dot(bright, vec3(1.0));
    bright = smoothstep(0.0, 0.5, vec3(c));

    gl_FragColor.rgb = mix(vec3(0), color, bright);
}
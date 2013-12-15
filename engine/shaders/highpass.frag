#version 420

uniform sampler2DMS renderedTexture;

uniform float threshold;

in vec2 uv;

void main()
{
    vec3 color = texelFetch(renderedTexture, ivec2(textureSize(renderedTexture) * uv), 0).rgb;

    // Bloom is converted to srgb to mask some aliasing on blur edges
    color = pow(color, vec3(1/2.2));

    vec3 bright = max(color - vec3(threshold), vec3(0));
    float c = dot(bright, vec3(1.0));
    bright = smoothstep(0.0, 0.5, vec3(c));

    gl_FragColor.rgb = mix(vec3(0), color, bright);
}
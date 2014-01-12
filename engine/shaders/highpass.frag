#version 420

uniform sampler2DMS renderedTexture;

uniform float threshold;

in vec2 uv;

void main()
{
    vec3 color = texelFetch(renderedTexture, ivec2(textureSize(renderedTexture) * uv), 0).rgb;

    vec3 brightColor = max(color - vec3(threshold), vec3(0.0));
    float bright = dot(brightColor, vec3(1.0));
    bright = smoothstep(0.0, 0.5, bright);

    gl_FragColor.rgb = mix(vec3(0.0), color, bright);
}
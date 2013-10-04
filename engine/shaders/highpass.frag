#version 400 core

uniform sampler2DMS renderedTexture;
in vec2 uv;

uniform float threshold;

void main() {
	ivec2 st = ivec2(textureSize(renderedTexture) * uv);

	vec3 color = texelFetch(renderedTexture, st, gl_SampleID).rgb;
	
	/*if (color.r > threshold || color.g > threshold || color.b > threshold)
		gl_FragColor = color;
	else
		gl_FragColor = vec4(0, 0, 0, 0);*/

    vec3 bright = max(color - vec3(threshold), vec3(0));
    float c = dot(bright, vec3(1));
    bright = smoothstep(0, 0.5, c);

    gl_FragColor.rgb = mix(vec3(0), color, bright);
}
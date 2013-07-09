#version 400 core

uniform sampler2DMS renderedTexture;
in vec2 uv;

uniform float brightThreshold;

void main() {
	ivec2 st = ivec2(textureSize(renderedTexture) * uv);

	vec4 color = texelFetch(renderedTexture, st, gl_SampleID);
	
	if (color.r > brightThreshold || color.g > brightThreshold || color.b > brightThreshold)
		gl_FragColor = color;
	else
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}
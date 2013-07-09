#version 330

uniform sampler2D gShadowMap;

in vec2 uv;

out vec4 fragColor;

void main()
{
	float depth = texture2D(gShadowMap, uv).x;
	depth = 1.0 - (1.0 - depth) * 10.0;
	fragColor = vec4(depth);
}
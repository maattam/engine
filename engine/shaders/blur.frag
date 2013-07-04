#version 330 core

uniform sampler2D tex;
uniform int width, height;

in vec2 uv;

void main()
{
	float dx = 1.0 / width;
	float dy = 1.0 / height;
	
	// Apply 3x3 gaussian filter
	vec4 color	 = 4.0 * texture2D(tex, uv);
	color		+= 2.0 * texture2D(tex, uv + vec2(+dx, 0.0));
	color		+= 2.0 * texture2D(tex, uv + vec2(-dx, 0.0));
	color		+= 2.0 * texture2D(tex, uv + vec2(0.0, +dy));
	color		+= 2.0 * texture2D(tex, uv + vec2(0.0, -dy));
	color		+= texture2D(tex, uv + vec2(+dx, +dy));
	color		+= texture2D(tex, uv + vec2(-dx, +dy));
	color		+= texture2D(tex, uv + vec2(-dx, -dy));
	color		+= texture2D(tex, uv + vec2(+dx, -dy));
	
	gl_FragColor = color / 16.0;
}
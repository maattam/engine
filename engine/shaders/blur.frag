#version 330 core

uniform sampler2D tex;
uniform int width, height;
uniform float lodLevel;

in vec2 uv;

    // 5x5 Gaussian kernel
    //   1  4  7  4 1
    //   4 16 26 16 4
    //   7 26 41 26 7 / 273
    //   4 16 26 16 4
    //   1  4  7  4 1

void main()
{
	float dx = 1.0 / width;
	float dy = 1.0 / height;
	
	// Apply 3x3 gaussian filter
	vec4 color	 = 4.0 * textureLod(tex, uv, lodLevel);
	color		+= 2.0 * textureLod(tex, uv + vec2(+dx, 0.0), lodLevel);
	color		+= 2.0 * textureLod(tex, uv + vec2(-dx, 0.0), lodLevel);
	color		+= 2.0 * textureLod(tex, uv + vec2(0.0, +dy), lodLevel);
	color		+= 2.0 * textureLod(tex, uv + vec2(0.0, -dy), lodLevel);
	color		+= textureLod(tex, uv + vec2(+dx, +dy), lodLevel);
	color		+= textureLod(tex, uv + vec2(-dx, +dy), lodLevel);
	color		+= textureLod(tex, uv + vec2(-dx, -dy), lodLevel);
	color		+= textureLod(tex, uv + vec2(+dx, -dy), lodLevel);
	
	gl_FragColor = color / 16.0;
}
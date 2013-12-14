#version 420

// http://homepages.inf.ed.ac.uk/rbf/HIPR2/gsmooth.htm
// 5x5 Gaussian kernel
//   1  4  7  4 1
//   4 16 26 16 4
//   7 26 41 26 7 / 273
//   4 16 26 16 4
//   1  4  7  4 1

uniform sampler2D inputTexture;
uniform int width, height;
uniform float lodLevel;

in vec2 uv;

vec4 sample5x5(in float dx, in float dy, int x, int y)
{
    return textureLod(inputTexture, uv + vec2((x - 2) * dx, (y - 2) * dy), lodLevel);
}

void main()
{
    float weights[25] = float[](    1, 4,  7,  4,  1,
                                    4, 16, 26, 16, 4,
                                    7, 26, 41, 26, 7,
                                    4, 16, 26, 16, 4,
                                    1, 4,  7,  4,  1);
	float dx = 1.0 / width;
	float dy = 1.0 / height;

    vec4 color = vec4(0);

    for(int i = 0; i < 25; ++i)
    {
        color += weights[i] * sample5x5(dx, dy, i % 5, i / 5);
    }

    color /= 273;
	
	gl_FragColor = color;
}
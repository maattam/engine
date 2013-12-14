#version 420

in vec3 texCoord;

out vec4 fragColor;

uniform sampler2DMS depth;
uniform samplerCube cubemap;
uniform float brightness;

subroutine void DepthTestType();
subroutine uniform DepthTestType depthTest;

subroutine(DepthTestType)
void skipDepthTest()
{
}

subroutine(DepthTestType)
void sampleDepthTest()
{
    float depth = texelFetch(depth, ivec2(gl_FragCoord.xy), 0).x;
    if(gl_FragCoord.z > depth)
    {
        discard;
    }
}

void main()
{
    depthTest();

    // Simulate HDR Skybox.. todo proper implementation using masks?
    fragColor = brightness * texture(cubemap, texCoord);
}
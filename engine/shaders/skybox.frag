//
//  Author   : Matti Määttä
//  Type     : Fragment shader
//  Summary  : Skybox shader for both forward and deferred pass.
//

#version 420

#define SAMPLES <>

in vec3 texCoord;

out vec4 fragColor;

uniform sampler2DMS depth;
uniform samplerCube cubemap;
uniform float brightness;

subroutine void DepthTestType();
subroutine uniform DepthTestType depthTest;

subroutine(DepthTestType)
void sampleDepthTest()
{
    float alpha = 1.0;
    ivec2 st = ivec2(gl_FragCoord.xy);

    // Blend skybox based on average visibility to reduce aliasing on skybox and mesh edges.
    for(int i = 0; i < SAMPLES; ++i)
    {
        float depth = texelFetch(depth, st, i).x;
        if(gl_FragCoord.z > depth)
        {
            alpha -= 1.0 / SAMPLES;
        }
    }

    // Skybox is completely occluded by geometry.
    if(alpha == 0.0)
    {
        discard;
    }

    fragColor.a = alpha;
}

subroutine(DepthTestType)
void skipDepthTest()
{
    fragColor.a = 1.0;
}

void main()
{
    depthTest();

    // Simulate HDR Skybox.. todo proper implementation using masks?
    fragColor.rgb = brightness * texture(cubemap, texCoord).rgb;
}
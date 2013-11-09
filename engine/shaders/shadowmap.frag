#version 420

uniform sampler2D gMaskSampler;

in vec2 texCoord0;

void main()
{
    bvec3 toDiscard = lessThan(texture2D(gMaskSampler, texCoord0), vec3(0.3));

    if(all(toDiscard))
        discard;
}
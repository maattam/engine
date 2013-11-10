#version 330 core

uniform sampler2D gMaskSampler;

in vec2 texCoord0;

void main()
{
    bvec3 toDiscard = lessThanEqual(texture2D(gMaskSampler, texCoord0), vec3(0.2));

    if(all(toDiscard))
        discard;
}